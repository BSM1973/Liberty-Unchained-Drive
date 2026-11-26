# UNCHAINED DRIVE — Manuel utilisateur
Liberty Music Software · v0.8.0 · Candidat de précommercialisation

## Installation et démarrage
macOS Apple Silicon : AU et VST3. Windows x64 : VST3 et application autonome. Les installateurs de cette version doivent être validés avant diffusion. Fermez votre DAW, installez le format voulu, relancez le logiciel et actualisez son navigateur de plugins. Sur macOS, les composants sont installés dans /Library/Audio/Plug-Ins/Components et les VST3 dans /Library/Audio/Plug-Ins/VST3. Windows : dossier VST3 standard du système. Ne supprimez pas les fichiers de vos projets pour désinstaller le plugin.
Placez Unchained Drive sur une piste audio, instrument ou un bus. Commencez avec un niveau modéré, Drive bas et Mix à 50 %. Ajustez le volume de sortie. Auto Gain facilite la comparaison mais ne remplace pas un limiteur. Aucun niveau de sortie n'est garanti.

## 1 — DRIVE
Choisissez un des dix circuits : Tube Heat (lampes), Tape Fire (bande), Transistor Edge (asymétrie), Console Color (console), Fuzz Chaos (fuzz), Diode Crush (clipping), Digital Destruction (quantification), Wavefolder X (repliement), Multiband Inferno (multibande), Hybrid Monster (deux circuits). Drive règle l'intensité; Entrée et Sortie règlent les gains; Mix dose le mélange; Bias déplace l'asymétrie; Tonalité, Couleur, Dynamique, Présence et Corps sculptent le signal.
Paramètres du circuit ouvre les profils et réglages spécifiques. Fuzz/Wavefolder : Anti-Alias arrondit les transitions, modifie le timbre et peut réduire certains résidus spectraux. Le mode historique est conservé. Hybrid : choisir A/B, Drive A/B, Blend, parallèle ou série et inversion d'ordre. Auto Gain utilise RMS, ou Pondération K si activée; LINK partage la correction en stéréo/M-S (Dual Mono reste indépendant).

## 2 — MULTIBAND INFERNO
Activer Inferno sélectionne le moteur. Choisir 3 ou 5 bandes. Glisser les séparations fréquentielles; chaque bande dispose de circuit, Drive, Mix, entrée, sortie, tonalité, couleur, dynamique, Solo et Mute. La phase linéaire utilise un FIR symétrique de 513 coefficients. Le retard fixe est de 256 échantillons plus celui du suréchantillonnage maximal déclaré.

## 3 — PRECISION TONE LAB
PRE EQ façonne le signal avant saturation (HP, LP, plateaux, deux bandes paramétriques). POST EQ propose quatre bandes paramétriques avec fréquence, gain, Q et activation. Déplacer les points sur la courbe. La courbe affichée représente le modèle de l'EQ post.

## 4 — DYNAMIC HEAT
Profondeur signée module le Drive selon l'enveloppe. Transient/Sustain règle la réponse aux variations rapides. Compression, attaque, relâchement, seuil et sensibilité permettent de contrôler la réponse. Une seconde enveloppe alimente la modulation.

## 5 — MOTION MATRIX
Cinq assignations : source, destination, profondeur signée. Sources : deux LFO, deux enveloppes, séquence et aléatoire. LFO : vitesse, forme, synchronisation au tempo et division. Destinations : Drive, Bias, Tonalité, Couleur, Mix, Sortie et Drive des cinq bandes. Les sources sont évaluées par bloc; les commandes principales sont lissées.

## 6 — SIGNAL INTELLIGENCE
Forme d'onde entrée retardée/sortie, spectre FFT 2048 points, courbe de transfert statique, Peak/RMS, LUFS M/S/I, LRA, estimation True Peak ×4. RESET réinitialise les statistiques intégrées et LRA; les fenêtres M/S continuent. DELTA compare la sortie à une référence filtrée et retardée; les effets temporels et de phase peuvent rester audibles.
TEST THD 1 kHz mesure une courbe statique isolée à 48 kHz et amplitude 0,3, avec l'oversampling temps réel. Aucun signal de test ne sort vers le DAW. Non disponible pour Hybrid/Multiband. Les mesures ne sont pas certifiées EBU.

## 7 — THE HEAT LIBRARY
100 presets d'usine dans dix collections, favoris, recherche (valider par Entrée ou bouton), navigation par pages et presets utilisateur. Nommer puis Enregistrer; Export/Import utilise le format .udpreset. INIT restaure les valeurs par défaut. Les presets usine peuvent modifier le caractère sonore; ajuster les niveaux à l'écoute.

## Commandes communes
A/B et copie comparent les réglages; Annuler/Rétablir; Bypass; menu langue Français/English (français par défaut). Clic droit sur une commande assignable, MIDI Learn, déplacer le contrôleur; le menu permet d'effacer l'assignation. L'hôte doit acheminer les CC MIDI vers l'effet. Les assignations sont sauvegardées dans l'état du plugin. Automatiser les paramètres exposés dans le DAW.
Oversampling OFF, ×2, ×4, ×8, ×16; qualité hors ligne indépendante si le DAW signale le rendu non temps réel. Une valeur plus haute coûte davantage de CPU. La latence est affichée en échantillons. Changement de qualité et de topologie à éviter pendant une prise critique.

## Limites et assistance
Les mesures sont des outils d'aide et non des certifications. La validation sonore, visuelle et DAW de cette version candidate doit être terminée avant vente. En cas de problème, relever OS, DAW/version, format, fréquence, buffer, preset et procédure de reproduction.
© Liberty Music Software — BREAK THE RULES. IGNITE YOUR SOUND.
