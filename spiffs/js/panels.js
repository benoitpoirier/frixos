/* ---------- collapsible panels ---------- */
document.querySelectorAll('.panel.collapsible > .ptitle').forEach(header => {
  const panel = header.parentElement;
  header.setAttribute('role', 'button');
  header.setAttribute('tabindex', '0');
  header.setAttribute('aria-expanded', String(!panel.classList.contains('collapsed')));

  function toggle() {
    const collapsed = panel.classList.toggle('collapsed');
    header.setAttribute('aria-expanded', String(!collapsed));
  }

  header.addEventListener('click', event => {
    const interactive = event.target.closest('a, button, input, select, textarea, summary, [role="button"], [role="switch"], [role="link"], [role="menuitem"], [tabindex]:not([tabindex="-1"])');
    if (interactive && interactive !== header) return;
    toggle();
  });
  header.addEventListener('keydown', event => {
    if (event.target !== header) return;
    if (event.key === 'Enter' || event.key === ' ') {
      event.preventDefault();
      toggle();
    }
  });
});
