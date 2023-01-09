# SY5 : Mini-rapport architecture de projet 'slash'
**CAHAGNE Marius**
**PICKERN Yann**
**POUILLART Lucas**

## Structure globale du projet

Nous avons décidé d'organiser notre projet autour d'un fichier slash.c qui contient la fonction main qui gère l'exécution du programme et la majorité de ses focntionnalités.
Nous avons également inclus d'autres fichiers de code source pour rendre le code plus lisible, notamment les fichiers mystring.h et mystring.c liés à la classe mystring que l'on a utilisée dans le TP6, qui permet de manipuler plus facilement les chaînes de caractères, ce que nous avons jugé utile.
Les fichiers command.c et command.h, qui constituent la classe command, contient toutes les fonctions et structures essentielles permettant de manipuler les commandes par slash.
Enfin, les fichiers wildcard.c et wildcard.h, qui constituent la classe wildcard, contiennent les fonctions essentielles à l'utilisation des jokers présents dans slash : `*` et `**`.

## Structure plus détaillée des différentes fonctionnalités

### Commandes internes pwd, cd et exit

Ces commandes là, qui sont gérées directement par le processus de slash, sont définies directement complètement dans slash.c à l'aide des fonction my_pwd, my_cd (et simplifyPath dans le cas des chemins logiques). Ces fonctions sont exécutées dans la fonction exec_cmd, qui est elle-même exécutée dans le parser.

my_cd change le répertoire courant et modifie les différentes variables en fonction des arguments.

my_pwd affiche soit le chemin "logique" stocké au fur et à mesure des changements de répertoire ou le chemin physique qu'on obtient à l'aide de la fonction getcwd().

### Commande exec_cmd
La fonction exec_cmd permet d'executer les commandes entrées par l'utilisateur en appelant les différentes fonctions précédentes en cas d'appel à cd ou pwd, de stopper l'exécution du programme en cas d'appel à exit, et en utilisant execvp pour les commandes externes. On étend les potentielles wildcards au début de la fonction. Nous n'avons malheureusement pas eu le temps d'implémenter les redirections.

### Gestion des wildcards
On a plusieurs fonctions permettant l'expansion des wildcards dans le fichier wildcard.c, la fonction expand_wildcard est la fonction principale, qui va vérifier si le chemin contient l'étoile simple *, l'étoile double ** ou un chemin normal. Si le chemin normal est valide, on l'ajoute aux arguments de la commande. A chaque rencontre avec une wildcard, on va la remplacer grâce à la fonction replace_wildcard, qui appelle la fonction expand_wildcard avec les chemins normaux où l'on a remplacé l'étoile avec chaque entrée correspondante. Enfin, On appelle la fonction addArgsAt qui ajoute les chemins correspondants à la commande.

### Gestion des signaux
On utilise SIG_IGN pour ignorer les signaux SIGINT et SIGTERM dans slash, puis on utilise SIG_DFL pour les refaire fonctionner normalement uniquement dans le processus fils. 




