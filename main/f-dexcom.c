#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "mbedtls/ssl.h"
#include "frixos.h"
#include "f-integrations.h"
#include "f-membuffer.h"
#include "f-json.h"
#include "f-dexcom.h"
#include "frixos.h"
#include "cJSON.h"
#include "f-display.h"

static const char *TAG = "f-dexcom";

// Dexcom API endpoints based on region
static const char *DEXCOM_BASE_URLS[] = {
    NULL,                          // Disabled
    "https://share2.dexcom.com",   // US
    "https://share2.dexcom.jp",    // Japan
    "https://shareous1.dexcom.com" // Rest of World
};

// Global variables
// Use unified glucose_data instead of separate dexcom_data
extern glucose_data_t glucose_data;
char dexcom_account_id[64] = {0};
char dexcom_session_id[64] = {0};

// Buffer for Dexcom responses
static char *dexcom_response_buffer = NULL;
static int dexcom_response_len = 0;
static size_t dexcom_response_buf_size = HTTP_BUFFER_SIZE;

// Persistent HTTP client for Dexcom
static esp_http_client_handle_t dexcom_client = NULL;
bool dexcom_client_initialized = false;

// Drop TLS/TCP state so the next perform() opens a new connection (recover from HTTP_EVENT_ERROR).
static void dexcom_reset_connection(void)
{
    if (dexcom_client)
        esp_http_client_close(dexcom_client);
}

// HTTP event handler for Dexcom requests
static esp_err_t dexcom_http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom HTTP error");
        dexcom_response_len = 0;
        break;
    case HTTP_EVENT_ON_DATA:
        // Ensure we have a valid buffer and the data will fit
        if (dexcom_response_buffer == NULL)
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Response buffer null");
            return ESP_FAIL;
        }

        // Check if we have enough space
        if (evt->data_len <= 0 || dexcom_response_len < 0 ||
            (dexcom_response_len + evt->data_len) >= (int)dexcom_response_buf_size)
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Buffer overflow len=%d data=%d max=%d",
                        dexcom_response_len, evt->data_len, (int)dexcom_response_buf_size);
            dexcom_response_len = 0;
            return ESP_FAIL;
        }

        // Safe to copy the data
        memcpy(dexcom_response_buffer + dexcom_response_len, evt->data, evt->data_len);
        dexcom_response_len += evt->data_len;
        dexcom_response_buffer[dexcom_response_len] = '\0';
        //ESP_LOG_WEB(ESP_LOG_INFO, TAG, "Dexcom HTTP Event: ON_DATA, data_len=%d, total_len=%d, data: %.*s", 
        //            evt->data_len, dexcom_response_len, evt->data_len, evt->data);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom HTTP Event: FINISH");
        break;
    default:        
        break;
    }
    return ESP_OK;
}

bool init_dexcom_client(void)
{
    const char *base_url = DEXCOM_BASE_URLS[eeprom_dexcom_region];
    if (!base_url)
        return false;

    glucose_data.trend_arrow = -1;

    // Initialize the client with base configuration
    esp_http_client_config_t config = {
        .url = base_url, // Just use the base URL initially
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
        .is_async = false,
        .crt_bundle_attach = custom_crt_bundle_attach,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .tls_version = ESP_TLS_VER_TLS_1_2,
        .user_agent = "Frixos HTTP Client",
        .event_handler = dexcom_http_event_handler,
    };

    dexcom_client = esp_http_client_init(&config);
    if (!dexcom_client)
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom client init failed");
        return false;
    }

    // Set common headers
    esp_http_client_set_header(dexcom_client, "Content-Type", "application/json");
    esp_http_client_set_header(dexcom_client, "Accept", "application/json");
    esp_http_client_set_header(dexcom_client, "connection", "close");

    return true;
}

void cleanup_dexcom_client(void)
{
    if (dexcom_client)
    {
        esp_http_client_cleanup(dexcom_client);
        dexcom_client = NULL;
    }
    dexcom_client_initialized = false;
}

bool authenticate_dexcom_account(void)
{
    ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom auth");
    const char *base_url = DEXCOM_BASE_URLS[eeprom_dexcom_region];
    if (!base_url)
        return false;

    // Initialize client if not already done
    if (!dexcom_client_initialized)
    {
        if (!init_dexcom_client())
        {
            return false;
        }
        dexcom_client_initialized = true;
    }

    char url[256];
    snprintf(url, sizeof(url), "%s/ShareWebServices/Services/General/AuthenticatePublisherAccount", base_url);

    // Acquire SSL connection semaphore before making SSL connection
    if (!acquire_ssl_semaphore("authenticate_dexcom_account"))
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Failed to acquire SSL semaphore for authenticate_dexcom_account");
        return false;
    }

    dexcom_response_buffer = get_shared_buffer(HTTP_BUFFER_SIZE, "DEXCOM_HTTP");
    if (!dexcom_response_buffer)
    {
        release_ssl_semaphore();
        return false;
    }
    dexcom_response_len = 0;

    // Update URL and method for this request
    esp_http_client_set_url(dexcom_client, url);
    esp_http_client_set_method(dexcom_client, HTTP_METHOD_POST);

    char auth_data[256];
    snprintf(auth_data, sizeof(auth_data),
             "{\"accountName\":\"%s\",\"password\":\"%s\",\"applicationId\":\"d89443d2-327c-4a6f-89e5-496bbb0317db\"}",
             eeprom_glucose_username, eeprom_glucose_password);

    esp_http_client_set_post_field(dexcom_client, auth_data, strlen(auth_data));

    bool success = false;
    if (esp_http_client_perform(dexcom_client) == ESP_OK)
    {
        if (esp_http_client_get_status_code(dexcom_client) == 200 && dexcom_response_len > 0)
        {
            // Response is the account ID, copy it
            int len = min(dexcom_response_len, sizeof(dexcom_account_id) - 1);
            memcpy(dexcom_account_id, dexcom_response_buffer, len);
            dexcom_account_id[len] = '\0';

            // Remove quotes
            if (dexcom_account_id[0] == '"')
            {
                memmove(dexcom_account_id, dexcom_account_id + 1, strlen(dexcom_account_id));
                dexcom_account_id[strlen(dexcom_account_id) - 1] = '\0';
            }
            ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom auth ok ID %s", dexcom_account_id);
            success = true;
        }
        else
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom account auth failed, status: %d", esp_http_client_get_status_code(dexcom_client));
            dexcom_reset_connection();
        }
    }
    else
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom auth HTTP failed");
        dexcom_reset_connection();
    }

    release_shared_buffer(dexcom_response_buffer);
    dexcom_response_buffer = NULL;
    
    // Always release SSL semaphore before returning
    release_ssl_semaphore();
    return success;
}

bool login_dexcom(void)
{
    ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom login");
    const char *base_url = DEXCOM_BASE_URLS[eeprom_dexcom_region];
    if (!base_url)
        return false;

    char url[256];
    snprintf(url, sizeof(url), "%s/ShareWebServices/Services/General/LoginPublisherAccountById", base_url);

    // Acquire SSL connection semaphore before making SSL connection
    if (!acquire_ssl_semaphore("login_dexcom"))
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "SSL lock failed (Dexcom login)");
        return false;
    }

    dexcom_response_buffer = get_shared_buffer(HTTP_BUFFER_SIZE, "DEXCOM_HTTP");
    if (!dexcom_response_buffer)
    {
        release_ssl_semaphore();
        return false;
    }
    dexcom_response_len = 0;

    // Update URL and method for this request
    esp_http_client_set_url(dexcom_client, url);
    esp_http_client_set_method(dexcom_client, HTTP_METHOD_POST);

    char auth_data[256];
    snprintf(auth_data, sizeof(auth_data),
             "{\"accountId\":\"%s\",\"password\":\"%s\",\"applicationId\":\"d89443d2-327c-4a6f-89e5-496bbb0317db\"}",
             dexcom_account_id, eeprom_glucose_password);

    esp_http_client_set_post_field(dexcom_client, auth_data, strlen(auth_data));

    bool success = false;
    if (esp_http_client_perform(dexcom_client) == ESP_OK)
    {
        ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom login response: %s", dexcom_response_buffer);
        if (esp_http_client_get_status_code(dexcom_client) == 200 && dexcom_response_len > 0)
        {
            int len = min(dexcom_response_len, sizeof(dexcom_session_id) - 1);
            memcpy(dexcom_session_id, dexcom_response_buffer, len);
            dexcom_session_id[len] = '\0';

            if (dexcom_session_id[0] == '"')
            {
                memmove(dexcom_session_id, dexcom_session_id + 1, strlen(dexcom_session_id));
                dexcom_session_id[strlen(dexcom_session_id) - 1] = '\0';
            }
            ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom login ok %s", dexcom_session_id);
            success = true;
        }
        else
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom login failed %d", esp_http_client_get_status_code(dexcom_client));
            dexcom_reset_connection();
        }
    }
    else
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom login HTTP failed");
        dexcom_reset_connection();
    }

    release_shared_buffer(dexcom_response_buffer);
    dexcom_response_buffer = NULL;
    
    // Always release SSL semaphore before returning
    release_ssl_semaphore();
    return success;
}

bool fetch_dexcom_glucose()
{
    // Initialize client if not already done
    if (!dexcom_client_initialized)
    {
        if (!init_dexcom_client())
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom client init failed");
            return false;
        }
        dexcom_client_initialized = true;
    }



    // Step 1: Get Account ID if we don't have it
    // this shouldn't be needed, but just in case
    if (dexcom_account_id[0] == '\0') {
        if (!authenticate_dexcom_account()) {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom fetch: no account ID");
            return false;
        }
    }


    for (int i = 0; i < 2; i++)
    { // Allow one retry on session failure
      // Step 2: Get Session ID if we don't have it
        if (dexcom_session_id[0] == '\0')
        {
            if (!login_dexcom())
            {
                ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom fetch: login failed");
                return false;
            }
        }

        // Step 3: Fetch glucose data
        const char *base_url = DEXCOM_BASE_URLS[eeprom_dexcom_region];
        char url[384];
        snprintf(url, sizeof(url), "%s/ShareWebServices/Services/Publisher/ReadPublisherLatestGlucoseValues?sessionId=%s&minutes=480&maxCount=48", base_url, dexcom_session_id);

        ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom fetch %s", url);
        
        // Acquire SSL connection semaphore before making SSL connection
        if (!acquire_ssl_semaphore("fetch_dexcom_glucose"))
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Failed to acquire SSL semaphore for fetch_dexcom_glucose");
            return false;
        }
        
        dexcom_response_buf_size = HTTP_BUFFER_SIZE * 2;
        dexcom_response_buffer = get_shared_buffer(dexcom_response_buf_size, "DEXCOM_HTTP");
        if (!dexcom_response_buffer)
        {
            release_ssl_semaphore();
            return false;
        }
        dexcom_response_len = 0;
        strcpy(dexcom_response_buffer, "EMPTY BUFFER");

        // Update URL and method for this request
        esp_http_client_set_url(dexcom_client, url);
        esp_http_client_set_method(dexcom_client, HTTP_METHOD_GET);
        // esp_http_client_set_post_field(dexcom_client, "", 0);

        bool success = false;
        if (esp_http_client_perform(dexcom_client) == ESP_OK)
        {
            int status_code = esp_http_client_get_status_code(dexcom_client);
            ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom fetch %d", status_code);
            if (status_code == 200)
            {
                cJSON *root = cJSON_Parse(dexcom_response_buffer);
                int arr_size = root && cJSON_IsArray(root) ? cJSON_GetArraySize(root) : 0;
                if (arr_size >= 1)
                {
                    /* --- Latest reading (current value + trend) --- */
                    cJSON *latest = cJSON_GetArrayItem(root, 0);
                    cJSON *value   = cJSON_GetObjectItem(latest, "Value");
                    cJSON *wt      = cJSON_GetObjectItem(latest, "WT");
                    cJSON *trend   = cJSON_GetObjectItem(latest, "Trend");

                    if (cJSON_IsNumber(value) && cJSON_IsString(wt))
                    {
                        glucose_data.previous_gl_mgdl = glucose_data.current_gl_mgdl;
                        glucose_data.current_gl_mgdl  = (float)value->valuedouble;
                        glucose_data.timestamp        = parse_dexcom_timestamp(wt->valuestring);

                        /* Previous reading for delta */
                        if (arr_size > 1)
                        {
                            cJSON *prev_val = cJSON_GetObjectItem(cJSON_GetArrayItem(root, 1), "Value");
                            if (cJSON_IsNumber(prev_val))
                                glucose_data.previous_gl_mgdl = (float)prev_val->valuedouble;
                        }
                        else
                        {
                            glucose_data.previous_gl_mgdl = glucose_data.current_gl_mgdl;
                        }
                        glucose_data.gl_diff = glucose_data.current_gl_mgdl - glucose_data.previous_gl_mgdl;

                        /* Trend arrow */
                        if (trend && cJSON_IsString(trend))
                        {
                            const char *ts = trend->valuestring;
                            if      (strcmp(ts, "DoubleDown")   == 0 || strcmp(ts, "SingleDown")  == 0) glucose_data.trend_arrow = 0;
                            else if (strcmp(ts, "FortyFiveDown") == 0)                                   glucose_data.trend_arrow = 1;
                            else if (strcmp(ts, "Flat")          == 0)                                   glucose_data.trend_arrow = 2;
                            else if (strcmp(ts, "FortyFiveUp")   == 0)                                   glucose_data.trend_arrow = 3;
                            else if (strcmp(ts, "SingleUp")      == 0 || strcmp(ts, "DoubleUp")   == 0) glucose_data.trend_arrow = 4;
                            else                                                                          glucose_data.trend_arrow = -1;
                        }
                        else
                        {
                            glucose_data.trend_arrow = -1;
                        }

                        /* --- Fill history (all entries, oldest-first order) --- */
                        time_t now_ts = time(NULL);
                        int hist_count = 0;
                        static glucose_history_point_t dexcom_hist[GLUCOSE_HISTORY_MAX];
                        for (int hi = 0; hi < arr_size && hist_count < GLUCOSE_HISTORY_MAX; hi++)
                        {
                            cJSON *item   = cJSON_GetArrayItem(root, hi);
                            cJSON *hval   = cJSON_GetObjectItem(item, "Value");
                            cJSON *hwt    = cJSON_GetObjectItem(item, "WT");
                            if (!cJSON_IsNumber(hval) || !cJSON_IsString(hwt)) continue;
                            time_t hts    = parse_dexcom_timestamp(hwt->valuestring);
                            int32_t age_m = (int32_t)((now_ts - hts) / 60);
                            if (age_m < 0 || age_m >= GLUCOSE_HISTORY_MINUTES) continue;
                            /* Insertion sort: keep oldest first */
                            int pos = hist_count;
                            while (pos > 0 && dexcom_hist[pos - 1].timestamp > hts) pos--;
                            memmove(&dexcom_hist[pos + 1], &dexcom_hist[pos],
                                    sizeof(glucose_history_point_t) * (hist_count - pos));
                            dexcom_hist[pos].gl_mgdl    = (float)hval->valuedouble;
                            dexcom_hist[pos].timestamp  = hts;
                            hist_count++;
                        }
                        memcpy(glucose_data.history, dexcom_hist,
                               sizeof(glucose_history_point_t) * hist_count);
                        glucose_data.history_count = (uint8_t)hist_count;

                        success = true;
                    }
                }
                if (root)
                    cJSON_Delete(root);

                if (success)
                {
                    release_shared_buffer(dexcom_response_buffer);
                    dexcom_response_buffer = NULL;
                    release_ssl_semaphore();
                    update_cgm_graph();
                    return true; // Success, exit loop
                }
            }
            else if (status_code == 500)
            { // could look for specific errors, like SessionIdNotFound, but what the heck, login again && strstr(dexcom_response_buffer, "SessionIdNotFound") != NULL) {
                ESP_LOG_WEB(ESP_LOG_WARN, TAG, "Dexcom session expired, re-login");
                dexcom_session_id[0] = '\0'; // Invalidate session, loop will retry
            }
            else
            {
                ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom glucose fetch returned http error: %d", status_code);
                dexcom_reset_connection();
                // Don't retry for other errors
                release_shared_buffer(dexcom_response_buffer);
                dexcom_response_buffer = NULL;
                release_ssl_semaphore();
                return false;
            }
        }
        else
        {
            ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom fetch failed");
            dexcom_reset_connection();
            dexcom_session_id[0] = '\0';
        }

        release_shared_buffer(dexcom_response_buffer);
        dexcom_response_buffer = NULL;
        release_ssl_semaphore();
    }

    ESP_LOG_WEB(ESP_LOG_ERROR, TAG, "Dexcom fetch failed (retry)");
    cleanup_dexcom_client();
    return false;
}

time_t parse_dexcom_timestamp(const char *timestamp)
{
    ESP_LOG_WEB(ESP_LOG_VERBOSE, TAG, "Dexcom timestamp %s", timestamp);
    // Dexcom timestamp format: "/Date(1234567890123-0700)/" 
    char *start = strstr(timestamp, "(");
    if (!start)
        return 0;

    char *end = strstr(start, "-");
    if (!end)
        end = strstr(start, ")");
    if (!end)
        return 0;

    char time_str[32];
    size_t len = end - start - 1;
    if (len >= sizeof(time_str))
        return 0;

    strncpy(time_str, start + 1, len);
    time_str[len] = '\0';

    return (time_t)(atoll(time_str) / 1000); // Convert milliseconds to seconds
}

// format_dexcom_token removed - now using unified format_glucose_token from f-freestyle.c