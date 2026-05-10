Ryane Menaï, Rapport projet freescord:

Ajout au protocol:
    Aucun ajout au protocol n'a eu lieu, surtout par manque de temps. J'ai
    préferé me concentrer sur la qualité de mon code plutôt que de faire des ajouts
    qui pourait potentiellement ajouter des bugs, même si j'aurais aimé pouvoir en faire plus.

Implémentation:
    - Côté serveur:
        Les données du serveur sont contenus dans une structure opaque.
        La fonction main alloue un serveur sur le tas, l'initialise
        puis execute la fonction srv_tick dans une boucle infinie.
        La fonction srv_tick accepte un potentiel client, et lui créer un thread.
        La communication avec l'user est ensuite gérer entièrement dans le thread
        créé.
    
    - Côté client:
        Même architecture qu'avec le serveur, les données sont contenus dans une
        structure opaque et les fonctions clt_* permettent d'utiliser cette structrure.
        La fonction main alloue, initialise puis execute clt_run sur le client.
        La fonction clt_run gère l'execution entière du client.
        Login dans le serveur puis reçois et envoie des messages au serveur,
        vérifie en premier lieu si les buffers ne sont pas vides, puis s'ils le sont,
        alors attends en utilisant poll.
    
    - Makefile:
        Je tiens à préciser que je suis déjà bien familié avec les makefiles,
        ceux figurants dans ce projets ont été intégrallement fait par moi même
        en utilisant mes propres connaissances acquises en lisant
        la documentation (https://www.gnu.org/software/make/manual/make.html) et avec ma propre expérience.

Ressources utilisées:
    Quelques pages de manuels de différentes fonctions d'API de socket et de strings.
    Ainsi que stackoverflow (https://stackoverflow.com/questions/60215078/c-printf-does-not-output-immediately).

Aide entre étudiants:
    Vérification mutuelles et discussions sur les différents algorithmes et solutions utilisées avec Sami Aït Meddour.
