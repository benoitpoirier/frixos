"""Playwright smoke tests for the Frixos web UI."""

from __future__ import annotations

import sys
from playwright.sync_api import sync_playwright

BASE = sys.argv[1] if len(sys.argv) > 1 else "http://127.0.0.1:8080"


def run() -> int:
    failures: list[str] = []
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        page = browser.new_page()
        page.goto(BASE, wait_until="networkidle")
        page.wait_for_timeout(1500)

        # Scrolling message loads from API
        msg = page.locator("#message")
        msg.wait_for(state="visible")
        val = msg.input_value()
        if not val:
            failures.append("Scrolling message textarea is empty on load (p16 bug)")
        else:
            print(f"PASS message loaded ({len(val)} chars)")

        # Tab navigation
        for tab in ("integrations", "status", "settings"):
            page.click(f'#nav a[data-tab="{tab}"]')
            page.wait_for_timeout(400)
            if not page.locator(f"#tab-{tab}").evaluate("el => el.classList.contains('active')"):
                failures.append(f"Tab {tab} did not activate")

        # Display brightness mode panels
        page.click('#nav a[data-tab="settings"]')
        display_panel = page.locator("#tab-settings .panel.collapsible").filter(
            has=page.locator("#dim_mode")
        )
        display_panel.locator(".ptitle").click()
        visibility_by_mode = {
            "1": (False, False, True, False),
            "0": (True, False, False, False),
            "2": (False, True, False, True),
        }
        selectors = ("#dimBright", "#dimTime", "#dimFullHint", "#brightness_LED0")
        for mode, expected in visibility_by_mode.items():
            page.select_option("#dim_mode", mode)
            for selector, visible in zip(selectors, expected):
                if page.locator(selector).is_visible() != visible:
                    failures.append(f"{selector} visibility is wrong for dim mode {mode}")

        # Language menu
        page.click("#langBtn")
        page.click('.lang-opt[data-lang="de"]')
        page.wait_for_timeout(500)
        if page.locator("#langName").inner_text() != "Deutsch":
            failures.append("Language switch to German failed")

        # Theme toggle
        was_dark = page.evaluate("document.body.classList.contains('theme-dark')")
        page.click("#themeBtn")
        page.wait_for_timeout(300)
        is_dark = page.evaluate("document.body.classList.contains('theme-dark')")
        if was_dark == is_dark:
            failures.append("Theme toggle did not change body class")

        browser.close()

    for f in failures:
        print(f"FAIL {f}")
    passed = 18 - len(failures)  # approximate check count
    print(f"\n{passed} checks passed, {len(failures)} failed")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(run())
