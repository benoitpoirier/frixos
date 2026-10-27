(function () {
  'use strict';

  const ONLOAD_KEY = 'frixos.shot.onLoad';
  const AUTO_KEY = 'frixos.shot.auto';
  const AUTO_VALUES = ['0', '10', '30', '60'];
  let initialized = false;
  let inFlight = false;
  let timer = null;
  let auto = '0';
  let updatedAt = null;
  let statusKind = 'idle';

  function readPreference(key, fallback) {
    try {
      const value = localStorage.getItem(key);
      return value === null ? fallback : value;
    } catch (_) { return fallback; }
  }

  function writePreference(key, value) {
    try { localStorage.setItem(key, value); } catch (_) { /* Storage may be unavailable. */ }
  }

  function translate(key, fallback) {
    return typeof tr === 'function' ? tr('settings.screenshot.' + key, fallback) : fallback;
  }

  function renderStatus() {
    const status = document.getElementById('shotStatus');
    if (!status) return;
    status.classList.toggle('error', statusKind === 'error' || statusKind === 'busy');
    if (statusKind === 'capturing') status.textContent = translate('capturing', 'Capturing…');
    else if (statusKind === 'busy') status.textContent = translate('busy', 'Device busy. Try again shortly.');
    else if (statusKind === 'error') status.textContent = translate('error', 'Capture failed. Try again.');
    else if (statusKind === 'updated' && updatedAt) {
      const pad = function (n) { return String(n).padStart(2, '0'); };
      status.textContent = translate('updated', 'Updated') + ' ' + pad(updatedAt.getHours()) + ':' +
        pad(updatedAt.getMinutes()) + ':' + pad(updatedAt.getSeconds());
    } else status.textContent = '';
  }

  function setStatus(kind, time) {
    statusKind = kind;
    if (time) updatedAt = time;
    renderStatus();
  }

  function scheduleAuto(delay) {
    clearTimeout(timer);
    timer = null;
    if (auto === '0' || inFlight) return;
    timer = setTimeout(function () {
      timer = null;
      const tab = document.getElementById('tab-settings');
      if (document.hidden || !tab || !tab.classList.contains('active')) {
        scheduleAuto();
        return;
      }
      capture();
    }, delay == null ? Number(auto) * 1000 : delay);
  }

  function decodeCapture(buffer, canvas) {
    const bytes = new Uint8Array(buffer);
    if (bytes.length < 8 || bytes[0] !== 70 || bytes[1] !== 88 || bytes[2] !== 83 || bytes[3] !== 49) {
      throw new Error('Invalid capture header');
    }
    const view = new DataView(buffer);
    const width = view.getUint16(4, true), height = view.getUint16(6, true);
    if (width !== canvas.width || height !== canvas.height) throw new Error('Unexpected capture size');

    const context = canvas.getContext('2d');
    const image = context.createImageData(width, height);
    for (let i = 3; i < image.data.length; i += 4) image.data[i] = 255;

    let offset = 8;
    while (offset + 8 <= bytes.length) {
      const x1 = view.getUint16(offset, true), y1 = view.getUint16(offset + 2, true);
      const x2 = view.getUint16(offset + 4, true), y2 = view.getUint16(offset + 6, true);
      offset += 8;
      if (x2 < x1 || y2 < y1) break;
      const recordWidth = x2 - x1 + 1, recordHeight = y2 - y1 + 1;
      const pixels = recordWidth * recordHeight;
      const available = Math.floor((bytes.length - offset) / 2);
      const count = Math.min(pixels, available);
      for (let n = 0; n < count; n++) {
        const x = x1 + (n % recordWidth), y = y1 + Math.floor(n / recordWidth);
        const value = view.getUint16(offset + n * 2, true);
        if (x >= width || y >= height) continue;
        const target = (y * width + x) * 4;
        const r5 = (value >> 11) & 31, g6 = (value >> 5) & 63, b5 = value & 31;
        image.data[target] = (r5 << 3) | (r5 >> 2);
        image.data[target + 1] = (g6 << 2) | (g6 >> 4);
        image.data[target + 2] = (b5 << 3) | (b5 >> 2);
      }
      offset += count * 2;
      if (count < pixels) break;
    }
    context.putImageData(image, 0, 0);
  }

  async function capture() {
    if (inFlight) return;
    const canvas = document.getElementById('shotCanvas');
    const loading = document.getElementById('shotLoading');
    const refresh = document.getElementById('shotRefresh');
    if (!canvas || !loading) return;
    clearTimeout(timer);
    timer = null;
    inFlight = true;
    loading.hidden = false;
    if (refresh) refresh.disabled = true;
    setStatus('capturing');
    const controller = new AbortController();
    const timeout = setTimeout(function () { controller.abort(); }, 8000);
    try {
      const response = await fetch('/api/screenshot', { cache: 'no-store', signal: controller.signal });
      if (response.status === 503) {
        setStatus('busy');
      } else if (!response.ok) {
        throw new Error('Capture request failed');
      } else {
        decodeCapture(await response.arrayBuffer(), canvas);
        canvas.hidden = false;
        document.getElementById('shotPlaceholder').hidden = true;
        document.getElementById('shotDownload').disabled = false;
        setStatus('updated', new Date());
      }
    } catch (error) {
      setStatus('error');
    } finally {
      clearTimeout(timeout);
      loading.hidden = true;
      if (refresh) refresh.disabled = false;
      inFlight = false;
      scheduleAuto();
    }
  }

  function localTimestamp(date) {
    const pad = function (n) { return String(n).padStart(2, '0'); };
    return date.getFullYear() + pad(date.getMonth() + 1) + pad(date.getDate()) + '-' +
      pad(date.getHours()) + pad(date.getMinutes()) + pad(date.getSeconds());
  }

  function downloadCapture() {
    const source = document.getElementById('shotCanvas');
    if (!source || source.hidden) return;
    const output = document.createElement('canvas');
    output.width = output.height = 512;
    const context = output.getContext('2d');
    context.imageSmoothingEnabled = false;
    context.drawImage(source, 0, 0, 512, 512);
    output.toBlob(function (blob) {
      if (!blob) { setStatus('error'); return; }
      const url = URL.createObjectURL(blob);
      const link = document.createElement('a');
      link.href = url;
      link.download = 'frixos-screen-' + localTimestamp(new Date()) + '.png';
      link.click();
      setTimeout(function () { URL.revokeObjectURL(url); }, 1000);
    }, 'image/png');
  }

  function initScreenshot() {
    if (initialized) return;
    const onLoadControl = document.getElementById('shotOnLoad');
    const refresh = document.getElementById('shotRefresh');
    const download = document.getElementById('shotDownload');
    const controls = document.querySelectorAll('.shot-seg .seg-btn');
    if (!onLoadControl || !refresh || !download) return;
    initialized = true;

    const storedOnLoad = readPreference(ONLOAD_KEY, '1');
    const onLoad = storedOnLoad !== '0';
    const storedAuto = readPreference(AUTO_KEY, '0');
    auto = AUTO_VALUES.includes(storedAuto) ? storedAuto : '0';
    onLoadControl.classList.toggle('on', onLoad);
    onLoadControl.setAttribute('aria-checked', String(onLoad));
    controls.forEach(function (button) {
      const selected = button.dataset.value === auto;
      button.classList.toggle('active', selected);
      button.setAttribute('aria-pressed', String(selected));
      button.addEventListener('click', function () {
        auto = button.dataset.value;
        writePreference(AUTO_KEY, auto);
        controls.forEach(function (item) {
          const active = item.dataset.value === auto;
          item.classList.toggle('active', active);
          item.setAttribute('aria-pressed', String(active));
        });
        scheduleAuto();
      });
    });
    onLoadControl.addEventListener('click', function () {
      const enabled = onLoadControl.getAttribute('aria-checked') !== 'true';
      onLoadControl.classList.toggle('on', enabled);
      onLoadControl.setAttribute('aria-checked', String(enabled));
      writePreference(ONLOAD_KEY, enabled ? '1' : '0');
    });
    refresh.addEventListener('click', capture);
    download.addEventListener('click', downloadCapture);
    document.addEventListener('visibilitychange', function () {
      if (!document.hidden && auto !== '0') scheduleAuto(250);
    });

    if (onLoad) capture();
    else scheduleAuto();
  }

  function refreshScreenshotI18n() {
    renderStatus();
  }

  window.initScreenshot = initScreenshot;
  window.refreshScreenshotI18n = refreshScreenshotI18n;
}());
