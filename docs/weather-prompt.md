Tu es designer de pixel art dans une équipe de developpement sur ESP32 avec un ecran de 128x128 pixels.
Tu dois realiser les sprits d'un widget meteo avec des sprites animés de 32x32pixels avec entre 20 et 40 frames.
Les 8 types de temps sont clearsky, fair, partly cloud, cloud, rain, storm, snow,fog.

**Dessin**
Les sprites sont optimisés au niveau des contours, des contrastes, des nombreuses nuances de couleurs facon pixel art afin que malgrès leur trés faible définition :
- les effets visuels soient agréable à regarder
- les formes soient clairement visibles
- on puisse rapidement identifier le type de temps
Les sprites sont réalisés sur fond noir. 
Les sprites sont en 3D et l'animation permet de faire ressortir la profondeur du dessin.

**Animation**
Chaque type de temps doit avoir un ou plusieurs effets visuels caractéristique de ce temps.
Le mouvements doivent paraitres fluide, et etre cyclique en bouclant de la dernière frame à la première sans que l'on puisse voir la transition.

**Sortie**
Je dois pouvoir voir avant la génération une preview des animations.
Afin de pouvoir intégrer ces sprites, je dois obtenir un spritesheet qui sera converti en tableau C au format RGB565.

**Déroulement de l'animation**
- Quelques soit le réglage utilisateur, une phase d'animation doit être jouée entièrement, elle ne peut etre coupée.
- Le tableau de frame d'une animation contient dans l'ordre  : 1 fois les frame du cycle de transition de début, x fois les frames du cycle nominale, 1 fois les frames du cycle de la transition de fin.
- L'utilisateur dispose d'un paramètre déjà en place pour le temps de pause entre 2 cycles complet (debut, nominal, fin), et un autre pour la durée totale des cycles nominaux.
- Lors d'une pause, la frame 0 de l'animation est affiché.
 
**Instructions complémentaires d'animation**
- Tous les sprites sont animés de manière significative.
- Tous les sprites on au moins une phase nominale de déroulement de l'animation.
- Tous les sprites composés d'un nuage ont un nuage dont les volumes 3D changes.
- Les temps partly cloud, rain, storm et snow sont décomposé en 2 éléments
  + le nuage 
  + les particules : pluies ou neige
- les sprites à particules sont décomposés en 3 phases
  + la transition de début permettant au sprite de passer de l'état 0 particules à N particules (15 frames)
  + la phase nominale permettant d'animer N particules (30 frames)
  + la transition de fin permettant au sprite de passer de l'état N particules à 0 particules (15 frames)
- la phase nominales permet d'animer en boucle N particules
- pour animer les N particules en boucle sans que la transition entre chaque phase soit imperceptible il faut qu'une particule lente parcours la hauteur de son point le plus haut à son point le plus bas dans la période d'une boucle, et pour les particules rapide dans la période d'une demi boucle.
- la dernière frame et la 1ere frame de la phase nominale s'enchaine sans que cela soit perceptible pour le spectateur
- les particules tombent depuis le milieu du nuage sur l'axe des Y et depuis une position aléatoire sur la première moitié l'axe des X, puis d'une position aléatoire sur la deuxieme moitié de l'axe de X
- elle disparaissent progressivement sur les dernier 5 pixels du bas du sprite
- pour les phase de début et nominale une fois une particule disparue tout en bas, elle repart tout en haut depuis son point d'origine afin d'obtenir une boucle sans fin
- pour la phase de fin, une fois une particule disparue tout en bas, elle disparait definitivement et ne réapparait plus
- lors de la phase de transition de debut, les particules apparaissent par 1 à 3 particules par frame en fonction de la quantité N de particules souhaitées en phase nominale

**Précision sur les nuages**
- Le relief des nuages et mis en valeur par des dégradés et des contours surlignés façon pixel art.
- ils sont composés d'au moins 4 voluptes superposés judicieusement de tailles différentes en relief.
- chaque petite volupte peut voir son diametre varier de 1px, les plus grosses de 2px
- le changement de colume des voluptes est non synchrone par rapport aux autres
- les voluptes supérieures sont beaucoup plus claires que les voluptes inférieures.
- le dessus des voluptes est beaucoup plus claire que le dessous

**Ajustement des animations**
Spécifications des 8 animations :
Ajuste toutes les animaions pour prendre en compte les instructions complémentaires.
Ajuste ces animations spécifiquement :
Partly Cloud : Nuage gris trés claire
Cloud (Nuageux) : Nuage Gris claire et 5 particules tombent lentement
Rain (Pluie) : Nuage Gris et des particules tombent rapidement en diagonale
Storm (Orage) : Nuage Gris et des particules tombent rapidement en diagonale. L'animation intègre éclairs vers le sol bleuté fin, un éclair zébré jaune épais apparaissant un peu plus longtemps, un flash bleuté à l'intérieur du nuage. Chaque éclair illume légérement tout le sprite et augmente le contraste et luminosité du nuage.
Snow (Neige) : Nuage Gris trés clair avec des flocons à 2 vitesses de descente, les lent sont fin (1 pixel blanc), les rapides sont gros 3 pixels de large x 3 pixels de haut en forme de + ou de x (avec des pixel blanc au centre et bleuté aux extrémités)
Fog (Brouillard) : L'animation doit montrer une dynamique des fluides lente et organique : une brume lourde qui dérive et rampe horizontalement. ABSOLUMENT AUCUN tourbillon, AUCUN vortex circulaire et AUCUNE forme géométrique ou sinusoïdale parfaite. Le brouillard est composé de volutes irrégulières et asymétriques et de nuages doux qui changent lentement de densité — s'étendant, se chevauchant et se dissipant de manière chaotique. Le style visuel doit suggérer plusieurs couches semi-transparentes se déplaçant dans des directions légèrement différentes pour créer une profondeur atmosphérique sombre. Ambiance ténébreuse et sombre. Palette de couleurs : fusain profond, bleus sombres de nuit, gris discrets et reflets à très faible opacité.




