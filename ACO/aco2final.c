#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_VILLES 10
#define MAX_ITERATIONS 10
#define MAX INT_MAX

typedef struct {
    char label; //nom de la ville
    double x; //x coordonee
    double y; //y coordonee
} Ville;

typedef struct {
    int ville1;
    int ville2;
} Arret;

typedef struct {
    Ville Villes[MAX_VILLES];
    Arret arrets[MAX_VILLES * (MAX_VILLES - 1)];
    double pheromones[MAX_VILLES][MAX_VILLES];
    int bestPath[MAX_VILLES];
    double bestPathLenth;
    int numVilles;
    double alpha; //coeff qui check influence des pheromones sur la proba
    double beta; //coeff qi check influence de la distance sur la proba
    double evaporationTaux;
    double initialPheromone;
} Graph;

double distance(Ville ville1, Ville ville2)
{
    double dx;
    double dy;
    double resultat;
    
    dx = pow(ville1.x - ville2.x, 2);
    dy = pow(ville1.y - ville2.y, 2);
    resultat = sqrt(dx + dy);
    return (resultat);
}

void arretsinit(Graph* graph)
{
    int arret_index;
    int i;
    int j;

    arret_index = 0;
    i = 0;
    while (i < graph->numVilles) 
    {
        j = 0;
        while (j < graph->numVilles)
        {
            if (i != j)
            {
                graph->arrets[arret_index].ville1 = i;
                graph->arrets[arret_index].ville2 = j;
                ++arret_index;
            }
            ++j;
        }
        ++i;
    }
}

void Graphinit(Graph* graph, double villes_coordinations[MAX_VILLES][2], int numVilles, double alpha, double beta, double evaporationTaux, double initialPheromone)
{
    int i;
    int j;
    int k;
    graph->numVilles = numVilles;
    graph->beta = beta;
    graph->evaporationTaux = evaporationTaux;
    graph->initialPheromone = initialPheromone;
    graph->alpha = alpha;

    i = 0;
    while (i < MAX_VILLES)
    {
        graph->Villes[i].label = 65 + i;
        graph->Villes[i].x = villes_coordinations[i][0];
        graph->Villes[i].y = villes_coordinations[i][1];
        i++;
    }
    arretsinit(graph);
    j = 0;
    while (j < MAX_VILLES)
    {
        k = 0;
        while (k < MAX_VILLES)
        {
            graph->pheromones[j][k] = initialPheromone;
            k++;
        }
        j++;
    }
    graph->bestPathLenth = MAX;
}

double calcule_distancetrajet(Graph *graph, int path[])
{
    double lenth = 0;
    int i = 0;

    while (i < graph->numVilles - 1)
    {
        // Accumulate the distance between two consecutive cities
        lenth += distance(graph->Villes[path[i]], graph->Villes[path[i + 1]]);
        i++;
    }

    // Add the distance between the last city and the first city
    lenth += distance(graph->Villes[path[graph->numVilles - 1]], graph->Villes[path[0]]);

    return lenth;
}

//hardest part:

// Fonction pour initialiser le tableau des villes restantes
void initialiserResteVille(Graph *graph, int resteVille[]) 
{
    int i;

    i = 0;
    while (i < graph->numVilles) 
    {
        resteVille[i] = i;
        ++i;
    }
}

// Fonction pour choisir la ville de départ aléatoirement
int choisirVilleDepart(Graph *graph) 
{
    return rand() % graph->numVilles;
}

// Fonction pour maj les proba en fct de la pheromone et de la distance
// calculer la probalite d aller vers une ville quelconque
//how fourmi take choice pour aller de la ville X a la ville Y
void calculerProbabilites(Graph *graph, int villeCourante, int resteVille[], double probalites[]) 
{
    int k ;
    int l ;
    double totalPheromone ;
    double distanceVersVille;
    double pheromone;

    k = 0;
    totalPheromone = 0;
    // traverson tt les villes du graph
    while (k < graph->numVilles) 
    {
        //si la ville nest pas visitee
        if (resteVille[k] != -1) 
        {
            //recherche dans la matrice de pheremone la valeur du pheromone entre la ville actuelle et la ville k
            pheromone = graph->pheromones[villeCourante][k];
            //la distance entre la ville courante et la ville k
            distanceVersVille = distance(graph->Villes[villeCourante], graph->Villes[k]);
            //calculer la probabilité d'une fourmi de choisir la ville k comme prochaine destination
            probalites[k] = pow(pheromone, graph->alpha) * pow(1 / distanceVersVille, graph->beta);
            //acuumuler le res
            totalPheromone += probalites[k];
        } 
        //si la ville est visitee
        else
            probalites[k] = 0;
        ++k;
    }
    // Normalisation cad somme soit egale à 1 pour les villes non visiite
    //if somme = 1 de toute les probas cad une distribution de proba valide
    l = 0;
    while (l < graph->numVilles) 
    {
        if (resteVille[l] != -1)
            probalites[l] = probalites[l] / totalPheromone;
        ++l;
    }
}
// Fonction pour choisir la prochaine ville en fonction des probabilites
int choisirProchaineVille(Graph *graph, double probalites[], int resteVille[], double randValeur) 
{ 
    //probabilites[] = tableau de proba normalise
    //randValeur comp entre 0 et 1
    int l;
    double probacumulatif;

    l = 0;
    probacumulatif = 0;
    while (l < graph->numVilles) 
    {
        if (resteVille[l] != -1) 
        {
            probacumulatif = probacumulatif + probalites[l];
            //si la ville est choisie alors return l
            if (randValeur <= probacumulatif)
                return l;
        }
        ++l;
    }
    // Protection de cas dune ville es choisie ou proba fausse calculer
    return -1;
}

// Fonct princip de la fourmi for building le chemin
void tourFourmi(Graph *graph, int path[]) 
{
    int resteVille[MAX_VILLES]; //tableau pour saver les ville non visitee cad a visitee
    int villeCourante; //ville curent de la ville
    int j;

    initialiserResteVille(graph, resteVille);//remplir le tableau des villes restante en utilsant la fct
    villeCourante = choisirVilleDepart(graph);//prendre la ville randomly
    path[0] = villeCourante;//la premiere ville du chemin et la ville courante 
    resteVille[villeCourante] = -1;//et dire que la ville choisie est visitee

    j = 1;
    while (j < graph->numVilles) 

    {
        double probalites[MAX_VILLES];
        calculerProbabilites(graph, villeCourante, resteVille, probalites);//on calcule La probabilite

        double randomValue = (double)rand() / RAND_MAX; //on initializ a une valeur random
        int prochaineVille = choisirProchaineVille(graph, probalites, resteVille, randomValue);//on chercher la ville suivante suivant les probas

        path[j] = prochaineVille;//le chemin donc sera de villecouurant jusqua la ville dapres till numVilles
        resteVille[prochaineVille] = -1;//et change sa valeur dans le tableau comme etante visite
        villeCourante = prochaineVille; //et en rfait
        j++;
    }
}

void updatePheromones(Graph *graph, int path[])
{
    double longeur_tour;
    double pheromoneDepot;
    int i;
    int ville1;
    int ville2;
    int derniere_ville;
    int premiere_ville;

    longeur_tour = calcule_distancetrajet(graph, path);
    pheromoneDepot = 1 / longeur_tour;
    i = 0;
    while (i < graph->numVilles - 1)
    {
        ville1 = path[i];
        ville2 = path[i + 1];
        graph->pheromones[ville1][ville2] += pheromoneDepot;
        graph->pheromones[ville2][ville1] += pheromoneDepot;
        i++;
    }
    //pour la derniere ville 
    derniere_ville = path[graph->numVilles - 1];
    premiere_ville = path[0];
    graph->pheromones[derniere_ville][premiere_ville] += pheromoneDepot;
    graph->pheromones[premiere_ville][derniere_ville] += pheromoneDepot;
}

void runACO(Graph *graph, int iterations) 
{
    int Fourmi;
    int iteration;
    int Fourmi_index;
    int i ;

    FILE *file = fopen("aco_output.csv", "w");
    if (file == NULL) 
    {
        fprintf(stderr, "Le fichier ne peut pas etre ouvert\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Iteration\t\tChemin\t\t\t\t\tLongeur\n");

    iteration = 0;
    while (iteration < iterations) 
    {
        int FourmiChemin[MAX_VILLES][MAX_VILLES];
        double FourrmiCheminLenth[MAX_VILLES];
        int bestPath[MAX_VILLES];
        double bestCheminLenth = INFINITY;

        Fourmi = 0;
        while (Fourmi < graph->numVilles) 
        {
            tourFourmi(graph, FourmiChemin[Fourmi]);
            FourrmiCheminLenth[Fourmi] = calcule_distancetrajet(graph, FourmiChemin[Fourmi]);

            if (FourrmiCheminLenth[Fourmi] < bestCheminLenth) 
            {
                bestCheminLenth = FourrmiCheminLenth[Fourmi];
                memcpy(bestPath, FourmiChemin[Fourmi], MAX_VILLES * sizeof(int));
            }
            Fourmi++;
        }
        updatePheromones(graph, bestPath);
        if (bestCheminLenth < graph->bestPathLenth) 
        {
            graph->bestPathLenth = bestCheminLenth;
            memcpy(graph->bestPath, bestPath, MAX_VILLES * sizeof(int));
        }

        Fourmi_index = 0;
        while (Fourmi_index < graph->numVilles) 
        {
            fprintf(file, "%d = \t", iteration + 1);
            i = 0;
            while (i < graph->numVilles) 
            {
                fprintf(file, "%c ", graph->Villes[FourmiChemin[Fourmi_index][i]].label);
                if (i < graph->numVilles - 1)
                    fprintf(file, " ");
                i++;
            }
            fprintf(file, "\t%lf\n", FourrmiCheminLenth[Fourmi_index]);
           Fourmi_index++;
        }
        iteration++;
    }
    fclose(file);
}

void printChemin(Graph *graph, int chemin[]) {
    int i;

    for (i = 0; i < graph->numVilles; ++i) {
        printf("%c", graph->Villes[chemin[i]].label);

        if (i < graph->numVilles - 1) {
            printf(" - ");
        }
    }

    printf("\n");
}


int main() {
    srand(time(NULL));

    double villes_coordinations[MAX_VILLES][2] = {
        {392.8, 356.4}, {559.6, 404.8}, {451.6, 186.0}, {698.8, 239.6},
        {204.0, 243.2}, {590.8, 263.2}, {389.2, 448.4}, {179.6, 371.2},
        {719.6, 205.2}, {489.6, 442.0}
    };

    Graph graph;
    Graphinit(&graph, villes_coordinations, 10, 1.0, 2.0, 0.5, 1.0);

    runACO(&graph, MAX_ITERATIONS);

    printf("\nBest path found: ");
    printChemin(&graph, graph.bestPath);
    printf("Best path length: %lf\n", graph.bestPathLenth);

    return 0;
}






/*graph->alpha :
Un coefficient qui contrôle l'influence des phéromones sur la probabilité.
Si alpha est élevé, les phéromones ont une influence plus significative sur la probabilité.*/

/*pow(pheromone, graph->alpha) :
Élève la quantité de phéromones à la puissance alpha.
Augmente l'influence des phéromones lorsque alpha est supérieur à 1.*/

/*pow(1 / distanceVersVille, graph->beta) :
Élève l'inverse de la distance à la puissance beta.
Réduit l'influence de la distance lorsque beta est supérieur à 1.*/