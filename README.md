# **FSH - Un mini-shell avec boucles for**

## **Description**
FSH est un interpréteur de commandes interactif qui permet d'exécuter des commandes externes et internes, de gérer les redirections et d'interpréter certaines structures conditionnelles et boucles.

## **Fonctionnalités principales**
✔️ **Exécution de commandes externes** en tenant compte du `$PATH`  
✔️ **Commandes internes** : `pwd`, `cd`, `exit`, `ftype`  
✔️ **Redirections des entrées/sorties** (`<`, `>`, `>>`, `2>`, `2>>`, etc.)  
✔️ **Pipelines** : `CMD_1 | CMD_2 | ... | CMD_N`  
✔️ **Structures conditionnelles** : `if CMD {}` / `if CMD {} else {}`  
✔️ **Boucles for avancées** avec filtres (`-e EXT`,`-A`, `-t TYPE`, `-p MAX`, `-r`, etc.)  
✔️ **Gestion du prompt** avec affichage du statut et du répertoire courant  
CMD est sois une commande interne sois un pipe ect...

## **Installation**
1. Cloner le dépôt :
   ```sh
   git clone https://github.com/Nidhalm1/shell-interactif.git
   ``` 
2. Se déplacer dans le dossier shell-interactif: 
   ```sh
   cd shell-interactif
   ```  
3. Compiler le programme :  
   ```sh
   make
   ```  
4. Lancer le shell :  
   ```sh
   ./fsh
   ```  


## **Exemple d'utilisation**
### **Exécuter une commande**  
```sh
ls -l
pwd
cd src
```  
### **Redirections**  
```sh
ls > fichier.txt       # Redirection de la sortie standard
ls >> fichier.txt      # Ajout à un fichier
ls 2> erreurs.log      # Redirection des erreurs
```  
### **Pipelines**  
```sh
ls -l | grep .sh | sort
```  
### **Boucles for**
Remplace `./dossier` par un chemin relatif pointant vers un dossier existant.
```sh
for F in ./dossier -e md { cat $F.md | grep MOUSSA }
```  

### **If TEST { CMD } else { CMD }**  
```sh
if ls { echo abc } else { echo acb }
```  
## **Exemple de prompt**  
```sh
[0] /home/user$
```  
Le `[0]` indique le code de retour de la dernière commande (0 = succès, 1 = échec).  


## **Effet des options de la boucle for**

- **-A** : Étend la portée de la boucle aux fichiers cachés (sauf . et ..).
- **-r** : Parcours récursif de toute l'arborescence de racine REP.
- **-e EXT** : Filtrage des résultats selon l'extension. Seuls les fichiers se terminant par .EXT sont pris en compte.
    - Exemple : `for F in . -e jpg { convert $F.jpg $F.png }`
- **-t TYPE** : Filtrage selon le type de fichier :
    - `f` : fichiers ordinaires
    - `d` : répertoires
    - `l` : liens symboliques
    - `p` : tubes
- **-p MAX** : Permet le traitement en parallèle d'un maximum de MAX tours de boucle.

## **Crédits**  
Développé dans le cadre du cours de Système en L3 Informatique.  

