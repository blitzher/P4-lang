import os
import tkinter as tk
from tkinter import E, LEFT, W, Entry, IntVar, ttk
import tkinter.filedialog as fd


class App(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        root.geometry("550x210")
        root.title("epicR - A recipe specification language")
        self.var = IntVar()
        self.var2 = IntVar()
        self.grid_configure(column=10, row=10)
        self.input_file_button = tk.Button(
            self,
            command=lambda: self.open_file(self.input_fpath),
            text="Choose file/Recipe",
            height=1,
            width=15,
        )
        self.output_dir_button = tk.Button(
            self,
            command=lambda: self.open_dir(self.out_dir),
            text="Choose output dir",
            height=1,
            width=15,
        )
        self.compile_btn = tk.Button(
            self,
            command=lambda: self.compile(),
            text="Compile output",
            height=1,
            width=15,
        )
        self.input_file_button.pack(anchor=W)
        self.output_dir_button.pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Fancy HTML style",
            variable=self.var,
            value=1,
            command=self.select_style,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Basic HTML style",
            variable=self.var,
            value=2,
            command=self.select_style,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Imperial system",
            variable=self.var2,
            value=1,
            command=self.select_system,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Metric system",
            variable=self.var2,
            value=2,
            command=self.select_system,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="No conversion of unit system",
            variable=self.var2,
            value=3,
            command=self.select_system,
        ).pack(anchor=W)
        self.compile_btn.pack(anchor=W)
        self.input_fpath = tk.StringVar()
        self.input_fpath.set("No file or recipe selected")
        self.input_fpath_label = tk.Label()
        self.input_fpath_label["textvariable"] = self.input_fpath
        self.output_fpath_label = tk.Label()
        self.out_dir = tk.StringVar()
        self.out_dir.set("No directory selected")
        self.output_fpath_label["textvariable"] = self.out_dir
        self.html_style = ""
        self.unit_system = ""

        self.input_fpath_label.place(x=150, y=3)
        self.output_fpath_label.place(x=150, y=33)

    def open_file(self, dest):
        file = fd.askopenfile(filetypes=[("epicR Files", "*.rcp")])

        if file:
            filepath = os.path.abspath(file.name)
            dest.set(self.format_fpath(filepath))

    def select_style(self):
        value = self.var.get()

        if value == 1:
            self.html_style = "Fancy"
        elif value == 2:
            self.html_style = "Basic"

    def select_system(self):
        value = self.var2.get()

        if value == 1:
            self.unit_system = "Imperial"
        elif value == 2:
            self.unit_system = "Metric"
        elif value == 3:
            self.unit_system = "None"

    def open_dir(self, dest):
        _dir = fd.askdirectory()

        if _dir:
            dirpath = os.path.abspath(_dir)
            dest.set(self.format_fpath(dirpath))

    def compile(self):

        if (os.name == "nt"):
            executable = os.path.join("bin", "main.exe")
        else:
            executable = os.path.join(".", "bin", "main")

        clargs = "%s -o %s --%s --%s" % (
            self.input_fpath.get(),
            self.out_dir.get(),
            self.html_style,
            self.unit_system,
        )

        os.system("%s %s" % (executable, clargs))

    def format_fpath(self, s):
        return s


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    app.mainloop()
