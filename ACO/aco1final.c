#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#define MAX_FOURMI 10
#define MAX_VILLES 10
#define ALPHA 0.1
#define RHO 0.5
#define Q 100.0
#define MAX INT_MAX

typedef struct Graph
{
    double pheromones[MAX_VILLES][MAX_VILLES];
    double distances[MAX_VILLES][MAX_VILLES];
} Graph;

typedef struct Fourmi
{
    int tour[MAX_VILLES];
    double tour_lenth;
} Fourmi;

double villes_coordinations[MAX_VILLES][2]={
    {392.8, 356.4}, {559.6, 404.8}, {451.6, 186.0}, {698.8, 239.6},
    {204.0, 243.2}, {590.8, 263.2}, {389.2, 448.4}, {179.6, 371.2},
    {719.6, 205.2}, {489.6, 442.0}
};

double calcul_distance(double x1, double y1, double x2, double y2)
{
    double dx;
    double dy;
    double resultat;

    dx = x2 - x1;
    dy = y2 - y1;
    resultat = sqrt(dx * dx + dy * dy);
    return (resultat);
}
void graph_init(Graph * graph)
{
    int i;
    int j;
    int k;
    int f;

    i = 0;
    while (i < MAX_VILLES)
    {
        j = 0;
        while (j < MAX_VILLES)
        {
            graph->pheromones[i][j] = ALPHA;
            graph->distances[i][j] = calcul_distance(
                villes_coordinations[i][0], villes_coordinations[i][1],
                villes_coordinations[j][0], villes_coordinations[j][1]);
            ++j;
        }
        ++i;
    }
    k = 0;
    printf("Matrice initiale de taille 10X10\n");
    while (k < MAX_VILLES)
    {
        f = 0;
        while (f < MAX_VILLES)
        {
            printf("%.4f\t", graph->pheromones[k][f]);
            ++f;
        }
        printf("\n");
        ++k;
    }
}

void fourmi_init(Fourmi fourmis[MAX_FOURMI])
{
    int i;
    int j;

    i = 0;
    while (i < MAX_FOURMI)
    {
        j = 0;
        while (j < MAX_VILLES)
        {
           fourmis[i].tour[j] = -1;
           ++j;
        }
        fourmis[i].tour_lenth = 0.0;
        ++i;
    }
}


double distance_entre_villes(Graph *graph, int ville1, int ville2) 
{
    return graph->distances[ville1][ville2];
}

int visiter(Fourmi *fourmi, int tour_index, int ville)
{
    int i;

    i = 0;
    while (i <= tour_index)
    {
        if (fourmi->tour[i] == ville)
            return 1;
        ++i;
    }
    return 0;
}

void travers_de_fourmi(Graph *graph, Fourmi *fourmi)
{
    int curt_ville;
    int i;
    int suiv_ville;
    int j;

    curt_ville = rand() % MAX_VILLES;
    fourmi->tour[0] = curt_ville;
    i = 0;
    while (i < MAX_VILLES - 1)
    {
        do
        {
            suiv_ville = rand() % MAX_VILLES;
        } while (visiter(fourmi, i, suiv_ville));
        ++i;
        fourmi->tour[i] = suiv_ville;
        fourmi->tour_lenth += distance_entre_villes(graph, fourmi->tour[i - 1], suiv_ville);
    }
    // ici c'est l'affichage du Path traversé avec la distance mise lors du travers pour chaque
    j = 0;
    while (j < MAX_VILLES)
    {
        printf("%d -- ", fourmi->tour[j]);
        ++j;
    }
    printf("\nLa Distance mise pour traverser est = %lf\n", fourmi->tour_lenth);
}
// A higher evaporation rate (closer to 1) would lead to faster decay of pheromones. 
void pheromone_matrice(Graph *graph, Fourmi fourmi[MAX_FOURMI], double mlr_tour, int iterat)
{
    int i;
    int j;
    int k;
    int n;
    int p;
    int q;
    int index_fourmi; //quel fourmis a le meuiller path
    double best_tour; //la comp de ca avec le tour de fourmi[0] sera vrai et donc fourmi[0] aura le mlr tour et ainsi de suite
    int ville1;
    int ville2;

    i = 0;
    while (i < MAX_VILLES)
    {
        j = 0;
        while (j < MAX_VILLES)
        {
            graph->pheromones[i][j] = graph->pheromones[i][j] * (1.0 * RHO);
            ++j;
        }
        ++i;
    }

    k = 0;
    index_fourmi = -1;
    best_tour = MAX;
    while (k < MAX_FOURMI)
    {
        double tour_pheromone_depot; //how much a specific ant deposed of pheromones 
        if (fourmi[k].tour_lenth > 0.0) //fourmi fait un tour qui nest pas 0
            tour_pheromone_depot = Q / fourmi[k].tour_lenth;
            // If the ant's tour is shorter, tour_pheromone_deposit will be larger.
            // If the ant's tour is longer, tour_pheromone_deposit will be smaller
        else
            tour_pheromone_depot = 1.0;

        n = 0;
        while (n < MAX_VILLES - 1)
        {
            ville1 = fourmi[k].tour[n];
            ville2 = fourmi[k].tour[n + 1];
            // remplir lintersection d V1 ET V2 avec le nouveau pheromone
            graph->pheromones[ville1][ville2] += tour_pheromone_depot;
            graph->pheromones[ville2][ville1] += tour_pheromone_depot;
            ++n;
        }
        // si la fourmi a le moins long chemin alors on sauvgarde ces donnees
        if (fourmi[k].tour_lenth < best_tour)
        {
            best_tour = fourmi[k].tour_lenth;
            index_fourmi = k;
        }
        ++k;
    }
    // affichage de la nouvelle matrice de pheromone
    p = 0;
    while (p < MAX_VILLES)
    {
        q = 0;
        while (q < MAX_VILLES)
        {
           printf("%.4f\t", graph->pheromones[p][q]);
           ++q;
        }
        printf("\n");
        ++p;
    }

    // affichage du meuilleur tour
    printf("\n Meuilleur tour etait pas la fourmi : %d, D une distance de = %lf\n", index_fourmi, best_tour);
}


double best_shorttour(Fourmi fourmi[MAX_FOURMI])
{
    double best;
    int i;

    best = fourmi[0].tour_lenth;
    i = 1;
    while (i < MAX_FOURMI) 
    {
        if (fourmi[i].tour_lenth > best)
            best = fourmi[i].tour_lenth;
        ++i;
    }
    return best;
}

int main()
{
    Graph graph;
    Fourmi fourmis[MAX_FOURMI];

    // Seed the random number generator only once
    srand(time(NULL));

    // Initialize graph and ants
    graph_init(&graph);
    fourmi_init(fourmis);

    // Perform ant tours and update pheromones
    for (int i = 0; i < MAX_FOURMI; ++i) {
        travers_de_fourmi(&graph, &fourmis[i]);
    }

    // Find the global best tour length and update pheromones
    double global_best_tour_length = best_shorttour(fourmis);
    pheromone_matrice(&graph, fourmis, global_best_tour_length, 0);

    return 0;
}




// The value of RHO being set to 0.5 is also a practical 
// choice because it represents a straightforward 
// interpretation: half of the pheromones decay in 
// each iteration. This makes it easier to reason about 
// the impact of the evaporation rate on the algorithm's 
// behavior.
