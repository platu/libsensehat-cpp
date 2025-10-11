# CHANGELOG — Migration libgpiod 1.x → 2.x

Date: 2025-10-09

## Contexte

Avec la migration de Debian Bookworm vers Debian Trixie, la version de la bibliothèque `libgpiod` a évolué de la série 1.x (par ex. 1.6.3) vers la série 2.x (par ex. 2.2.1). Cette migration introduit des changements d'API importants : la gestion des lignes GPIO s'effectue maintenant via des objets de configuration et de requête (`gpiod_line_settings`, `gpiod_line_config`, `gpiod_line_request`) plutôt qu'avec les fonctions simples de la v1 (`gpiod_chip_get_line`, `gpiod_line_request_output`, `gpiod_line_set_value`, etc.).

## Changements effectués dans ce dépôt

Fichiers modifiés/ajoutés :

- `src/sensehat.cpp`

  - Migration complète de l'API libgpiod v1 → v2 :
    - Remplacement des types et fonctions v1 par l'API v2 `line_request`.
    - Remplacement du tableau `struct gpiod_line *gpio_line[]` par
      `struct gpiod_line_request *gpio_line[]`.
    - Ajout d'un helper `_request_line(pin, direction)` qui encapsule la
      création de `gpiod_line_settings` et `gpiod_line_config` et qui
      appelle `gpiod_chip_request_lines()`.
    - Remplacement des appels de lecture/écriture par
      `gpiod_line_request_get_value(request, offset)` et
      `gpiod_line_request_set_value(request, offset, value)`.
    - Ajout d'un helper `_open_first_gpiochip()` pour détecter et ouvrir
      dynamiquement le premier `/dev/gpiochipN` disponible plutôt que
      d'utiliser un chemin codé en dur.
    - Libération systématique des `gpiod_line_request` via
      `gpiod_line_request_release()` dans `senseShutdown()`.

- `include/LSM9DS1_Types.h`
  - Ajout de `#include <cstdint>` pour corriger des erreurs de compilation
    liées à `uint8_t`/`uint16_t` lors de la construction des exemples.

## Raison du changement

- La v2 de libgpiod modifie l'interface publique pour être plus expressive
  et permettre des demandes groupées de lignes, la configuration fine des
  paramètres de ligne (bias, drive, active low, etc.) et des requêtes
  atomiques sur plusieurs lignes. Le code existant utilisant la v1 ne
  compile plus correctement sous libgpiod 2.x.

## Impact et compatibilité

- Le code modifié est compatible avec libgpiod >= 2.x.
- Si vous devez maintenir la compatibilité avec des systèmes utilisant
  libgpiod 1.x, envisagez :
  - d'ajouter une couche d'adaptation conditionnelle via des macros
    (#ifdef) pour détecter la version disponible et utiliser l'API
    appropriée, ou
  - de documenter clairement dans le README la version minimale
    requise (libgpiod >= 2.0.0) pour la branche actuelle.

## Tests recommandés

1. Compiler la bibliothèque et les exemples :

```bash
make
```

2. Vérifier que la compilation réussit sans erreurs. (Le dépôt a été
   testé localement et compile correctement après les modifications.)

3. Tester au moins un exemple qui manipule des GPIOs en conditions
   réelles (sur la Raspberry Pi) : par exemple `examples/08_gpioOutputBlink`
   (ou `06_setPixelToNorth` si vous utilisez des GPIOs dans cet exemple).

```bash
# Exemple : exécuter le clignotement GPIO (nécessite droits)
cd examples
./08_gpioOutputBlink
```

4. Si l'exécution échoue à cause de permissions, ajoutez votre utilisateur
   au groupe `gpio` ou exécutez l'exemple avec `sudo`.

## Notes d'implémentation

- La nouvelle API requiert que vous fournissiez les offsets des lignes
  lorsque vous lisez ou écrivez leur valeur (p. ex. `gpiod_line_request_set_value(request, offset, value)`).
- Les helper ajoutés dans `src/sensehat.cpp` (`_request_line`,
  `_open_first_gpiochip`) ont été conçus pour rester simples et compréhensibles
  pour les étudiants; ils peuvent être améliorés si vous avez besoin de
  fonctionnalités avancées (pull-up/down, debouncing, edge detection...).

## Recommandation

- Mettre à jour la documentation (`README.md`) avec une ligne indiquant
  que la version minimale requise de libgpiod est désormais la 2.x.
- Si vous distribuez ce code à des étudiants ou utilisateurs, mentionnez
  explicitement la procédure d'installation de libgpiod (package system)
  et l'appartenance au groupe `gpio` pour l'accès sans sudo.

---

Si vous voulez, je peux :

- ajouter un contrôle préprocesseur pour supporter à la fois v1 et v2 ;
- ajouter une note dans `README.md` et/ou un changelog au format plus
  structuré (Keep a Changelog) ;
- documenter la procédure d'installation pour Debian Trixie.

Dites-moi quelle option vous préférez et je l'implémenterai.
