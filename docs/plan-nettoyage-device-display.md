# Plan : retirer Fonts et Layout/Load de l’onglet Device

## Objectif

Supprimer de **Device → Display** le bloc **Fonts** (raccourci de police jour/nuit) et la ligne **Layout + Load**. L’onglet **Layout** reste le seul endroit pour choisir les polices et charger une disposition.

## À ne pas modifier

| Élément | Raison |
|---|---|
| Firmware (`main/*.c`), NVS `dayfont`/`nightfont`, clés `p04`/`p05`, `/api/screen` et `/api/files` | Utilisés par l’onglet Layout, l’affichage et le sélecteur de police de l’intégration Home Assistant (voir `docs/developer-guide.md`, section « Select »). |
| `appendScreenFontSamples`, `createFontSamplePreview`, `SCREEN_TIME_FONTS`, `fetchScreenLayout`, `refreshScreenLayoutFiles`, `renderScreenPresets`, `loadSystemScreenLayout` et `encodeScreenLayoutBinary` | Utilisés par l’onglet Layout. |
| Synchronisation de `window.settings.p04/p05` dans Apply de Layout (`spiffs/js/screen-editor.js`) | Nécessaire pour maintenir le cache des réglages à jour. |
| CSS `.city-row`, `details.adv-box` et `.screen-font-sample*` | Utilisé ailleurs, notamment dans l’onglet Layout. |
| Clés i18n `menu.screen`, `messages.saving_settings`, `messages.layout_read_invalid` et `messages.error_saving_settings` | Encore utilisées ailleurs. |
| `frixos-ui/` | Copie séparée, sans référence aux contrôles concernés. |
| `idf.py build` | Ne pas lancer de build pour cette tâche. |

## Étape 1 — HTML : `spiffs/index.html`

Dans le panneau Display, supprimer uniquement les deux blocs suivants :

1. `<details class="adv-box" id="fontsBox">…</details>`, y compris `#dayFontSamples` et `#nightFontSamples`.
2. Le `<div class="field">` de Layout qui contient `#settingsLayoutSelect` et `#settingsLayoutLoadBtn`.

Résultat attendu : **Display rotation** est suivi directement de **Mirror display**. Ne pas supprimer le lien de navigation vers l’onglet Layout (`<a data-tab="layout" data-i18n="menu.screen">`).

## Étape 2 — JavaScript : `spiffs/js/forms.js`

1. Dans `loadSettings()`, supprimer l’appel à `populateSettingsLayoutSelect()`.
2. Supprimer la déclaration de `settingsLayoutLoadBtn` et son gestionnaire de clic.
3. Dans `sectionLoaders.settings`, supprimer l’appel à `initFontsShortcut()`.

## Étape 3 — JavaScript : `spiffs/js/core.js`

Dans `refreshDynamicI18n()`, supprimer le commentaire concernant le menu Layout de Display et le bloc qui appelle `fillLayoutSelect()` pour `settingsLayoutSelect`.

## Étape 4 — JavaScript : `spiffs/js/screen-editor.js`

Supprimer ces fonctions et leurs commentaires dédiés, une fois leurs appels retirés aux étapes précédentes :

| Fonction | Commentaire associé |
|---|---|
| `fillLayoutSelect(selectId)` | `// Fill a <select> with "Current"…` |
| `populateSettingsLayoutSelect()` | `// Settings > Display "Layout" dropdown…` |
| `applySettingsLayoutToDevice()` | Aucun |
| `applyFontShortcut(mode, font, box, grid)` | Bloc `/* ---------- Fonts shortcut (Device > Display) ---------- … */` |
| `buildFontSampleGrid(mode, current)` | Aucun |
| `renderFontsShortcut()` | Aucun |
| `initFontsShortcut()` | Aucun |

Dans le commentaire au-dessus de `renderScreenPresets`, remplacer la référence à « the Load button » par une description autonome, par exemple :

> Clicking a card loads that layout into the editor; the user then presses Apply.

**Conserver** `refreshScreenLayoutFiles` : `fetchScreenLayout()` l’utilise.

## Étape 5 — JavaScript : `spiffs/js/utils.js`

Dans `translations.en.messages`, supprimer les fallbacks `layout_applied` et `layout_apply_failed`, utilisés uniquement par `applySettingsLayoutToDevice()`.

## Étape 6 — Traductions : `spiffs/i18n/`

Dans chacun des neuf fichiers `language_{da,de,en,es,fr,it,pl,pt,sv}.json`, supprimer :

- `advanced.fonts` au complet (`title`, `desc`, `day`, `night`) ;
- `common.load` et `common.current` ;
- `messages.font_applied`, `messages.layout_applied` et `messages.layout_apply_failed`.

**Avant de supprimer une clé**, rechercher ses références dans `spiffs/js/` et `spiffs/index.html`. Si une clé est encore utilisée, la conserver. Vérifier également les virgules pour que chaque fichier reste un JSON valide.

## Étape 7 — Test Playwright : `tools/test_ui_playwright.py`

Le test identifie actuellement le panneau Display avec `has=page.locator("#fontsBox")`. Remplacer ce sélecteur par `has=page.locator("#dim_mode")`.

## Étape 8 — Vérifications

1. Les recherches suivantes ne doivent plus trouver de références dans `spiffs/` et `tools/` :

   ```text
   fillLayoutSelect|populateSettingsLayoutSelect|applySettingsLayoutToDevice|settingsLayoutSelect|settingsLayoutLoadBtn
   initFontsShortcut|renderFontsShortcut|buildFontSampleGrid|applyFontShortcut|fontsBox|dayFontSamples|nightFontSamples
   ```

2. Vérifier la syntaxe JavaScript :

   ```sh
   node --check spiffs/js/forms.js &&
   node --check spiffs/js/core.js &&
   node --check spiffs/js/screen-editor.js &&
   node --check spiffs/js/utils.js
   ```

3. Vérifier que les fichiers de traduction sont des JSON valides :

   ```sh
   python -c "import json,glob;[json.load(open(f,encoding='utf-8')) for f in glob.glob('spiffs/i18n/*.json')]"
   ```

4. Lancer le test du contrat UI :

   ```sh
   python tools/test_ui_settings.py
   ```

5. Avec le serveur de test décrit dans `docs/developer-guide.md`, vérifier dans le navigateur :
   - Device → Display n’affiche plus Fonts ni Layout/Load ;
   - aucune erreur JavaScript n’apparaît dans la console ;
   - Save et le changement de langue fonctionnent toujours ;
   - l’onglet Layout affiche toujours ses presets, permet de choisir **Theme/Font**, **Aux digit theme** et les échantillons de police, puis applique la disposition.

## Documentation et déploiement

Aucune modification du README ou des autres documents n’est prévue : ces raccourcis n’y sont pas décrits. Les clés `p04`/`p05` documentées dans `docs/kb-http-api.md` restent valides.

Le changement concerne l’interface dans `spiffs/` et un test outil. Le firmware n’est pas modifié ; le déploiement nécessite de mettre à jour SPIFFS.

## Résultat attendu

Retirer les raccourcis redondants de Device sans supprimer les fonctionnalités de l’onglet Layout ni les chemins firmware encore nécessaires. Ne pas modifier le firmware.
