# UNCHAINED DRIVE — Liberty Music Software

Version de reconstruction initiale 0.1.0 — source C++17 / JUCE 8.0.6. Ce dépôt est une base de développement, pas une version commerciale validée.

## Compilation

Installer CMake 3.22+, Xcode (macOS) ou Visual Studio 2022 avec C++ (Windows), et disposer d'un accès réseau à GitHub pour télécharger JUCE. La compilation macOS ARM64 génère AU, VST3 et Standalone ; Windows x64 génère VST3 et Standalone.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Sous Windows : `cmake -S . -B build -A x64`, puis les deux dernières commandes ci-dessus. Le workflow `.github/workflows/build.yml` est prévu pour déclencher les deux builds à chaque push.

Test DSP indépendant de JUCE (Linux/macOS avec un compilateur C++17) :

```sh
c++ -std=c++17 -O2 Tests/core_test.cpp -o core_test
./core_test
```

Résultat observé localement : 40 essais (10 moteurs × 4 fréquences d'échantillonnage) + contrôles dry et bypass réussis. Ce test vérifie essentiellement la finitude du signal, son énergie et les conditions de passage direct ; il ne valide ni qualité sonore ni performances temps réel.

## Contenu

- `Source/Core.h` : dix chemins DSP, enveloppe, multibande trois bandes, hybride, pré/post EQ, coloration, mix.
- `Source/Plugin.*` : APVTS, 100 configurations d'usine, MIDI CC Learn avec assignations persistantes dans l'état du projet, modulation à cinq routes, suréchantillonnage JUCE prévu de 1x à 16x, modes stéréo/mid-side/double mono, état et comparaison A/B.
- `Source/Editor.cpp` : sept pages JUCE dessinées en vectoriel, français par défaut, anglais via réglages, contrôles métalliques, courbes, FFT et mesures de sortie. Les favoris et presets utilisateur sont prévus dans le dossier de données de l'application.
- `References/` : sept captures originales, uniquement pour contrôle de fidélité, non utilisées comme fonds d'interface.

## Limites et travaux avant commercialisation

- JUCE n'a pas pu être téléchargé dans l'environnement de création : le wrapper et l'UI n'ont pas été compilés ni rendus à l'écran. Les builds natifs et la fidélité graphique restent à vérifier. Aucun installateur ou fichier AU/VST3 compilé n'est inclus.
- Les modèles DSP sont des approximations initiales originales, pas des émulations circuit par circuit. Les paramètres de circuit détaillés (vitesse de bande, wow/flutter, types de diodes, profils console, starvation, etc.) sont à développer. Le multibande reste à trois bandes avec séparation complémentaire simple ; cinq bandes et phase linéaire non implémentées. L'hybride possède des paramètres de routage internes, dont l'interface complète reste à réaliser.
- Les 100 presets sont des configurations paramétrées distinctes générées par catégories, non auditées individuellement sur du matériel audio. Le preset Init est hors des 100.
- Le gain automatique actuel est une approximation statique, pas un véritable appariement RMS/LUFS. La télémétrie représente la sortie seulement ; analyse entrée/sortie, LUFS, résidu harmonique, réduction de gain et mesures CPU restent à développer.
- Lissage continu, bypass et changements de suréchantillonnage sans clics, précision de la compensation de latence, FFT à toutes fréquences, optimisations CPU, modulation à précision échantillon, glisser-déposer de modulation, séquenceur éditable, qualité offline séparée, verrouillage des paramètres et routage MIDI hôte restent à valider ou à compléter. Les CC MIDI sont relayés sur le thread de messages : pas de garantie de précision à l'échantillon.
- Localisation de la plupart des commandes visibles, mais certains noms de modules, de presets, de moteurs et choix techniques restent en anglais. Contrôler la typographie, l'absence de glyphes manquants, les chevauchements, et l'écran 13 pouces sur les builds réels. Les captures sont les références strictes ; la reproduction initiale ne prétend pas être pixel-parfaite.
- Intégration native Liberty DAW, signature Developer ID, notarisation et installateurs Liberty Release Center : non réalisés dans cette version. Ne pas modifier BSM-DAW avant validation du plugin autonome.
