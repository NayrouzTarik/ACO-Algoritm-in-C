import tkinter as tk
from tkinter import ttk
from itertools import permutations
import random

class ACOVisualization(tk.Tk):
    def __init__(self, cities, iterations):
        super().__init__()
        self.title("ACO Graph Visualization")
        self.geometry("800x800")

        self.cities = cities
        self.iterations = iterations

        self.generate_paths()

        self.notebook = ttk.Notebook(self)
        self.notebook.pack(fill=tk.BOTH, expand=True)

        self.draw_aco_graph()

        for i in range(min(5, len(self.paths))):
            path_info = self.paths[i]
            path, length = path_info
            self.display_path(path, length, i + 1)

    def draw_aco_graph(self):
        aco_tab = ttk.Frame(self.notebook)
        self.notebook.add(aco_tab, text="ACO Graph")

        aco_canvas = tk.Canvas(aco_tab, width=800, height=800)
        aco_canvas.pack()
        
        common_cities = set(self.paths[0][0]).intersection(*[set(path[0]) for path in self.paths])


        for city in common_cities:
            coords = self.cities[city]
            aco_canvas.create_oval(coords[0] - 10, coords[1] - 10, coords[0] + 10, coords[1] + 10, fill="blue")
            aco_canvas.create_text(coords[0], coords[1] - 15, text=str(city))

        for path_info in self.paths:
            path, _ = path_info
            for i in range(len(path) - 1):
                start_city = path[i]
                end_city = path[i + 1]
                start_coords = self.cities[start_city]
                end_coords = self.cities[end_city]
                aco_canvas.create_line(start_coords[0], start_coords[1], end_coords[0], end_coords[1], arrow=tk.LAST, fill="black")

 
        for i in range(self.iterations):
            aco_canvas.create_text(400, 780 + i * 15, text=f"Iteration {i + 1}")

    def generate_paths(self):
        self.paths = []
        cities_list = list(self.cities.keys())


        subset_paths = random.sample(list(permutations(cities_list)), 20)  

        for path in subset_paths:
            length = random.uniform(500, 1000) 
            self.paths.append((list(path), length))

    def display_path(self, path, length, window_number):
        path_tab = ttk.Frame(self.notebook)
        self.notebook.add(path_tab, text=f"Path {window_number}")

        path_canvas = tk.Canvas(path_tab, width=800, height=800)
        path_canvas.pack()

        self.draw_cities_and_edges(path_canvas, path)

        info = f"Path: {' '.join(path)}, Length: {length:.2f}"
        path_canvas.create_text(400, 780, text=info, fill="red")

    def draw_cities_and_edges(self, canvas, path):
        for city, coords in self.cities.items():
            canvas.create_oval(coords[0] - 10, coords[1] - 10, coords[0] + 10, coords[1] + 10, fill="blue")
            canvas.create_text(coords[0], coords[1] - 15, text=str(city))

        for i in range(len(path) - 1):
            start_city = path[i]
            end_city = path[i + 1]
            start_coords = self.cities[start_city]
            end_coords = self.cities[end_city]
            canvas.create_line(start_coords[0], start_coords[1], end_coords[0], end_coords[1], arrow=tk.LAST, fill="red")

if __name__ == "__main__":
    cities = {
        '1': (392.8, 356.4),
        '2': (559.6, 404.8),
        '3': (451.6, 186.0),
        '4': (698.8, 239.6),
        '5': (204.0, 243.2),
        '6': (590.8, 263.2),
        '7': (389.2, 448.4),
        '8': (179.6, 371.2),
        '9': (719.6, 205.2),
        '10': (489.6, 442.0)
    }

    iterations = 10

    app = ACOVisualization(cities, iterations)
    app.mainloop()
