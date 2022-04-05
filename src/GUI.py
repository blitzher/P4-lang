import os
import tkinter as tk
from tkinter import IntVar, ttk
import tkinter.filedialog as fd


class App(tk.Frame):

    def __init__(self, master):
        super().__init__(master)
        root.geometry("550x150")
        root.title("epicR - A recipe specification language")
        self.var = IntVar()
        self.grid_configure(column=10, row=10)
        self.input_file_button = tk.Button(
            self, command=lambda: self.open_file(self.input_fpath), text="Open file", height=1, width=15)
        self.output_dir_button = tk.Button(
            self, command=lambda: self.open_dir(self.out_dir), text="Open output dir", height=1, width=15)
        self.compile_btn = tk.Button(
            self, command=lambda: self.compile(), text="Compile output", height=1, width=15)
        self.input_file_button.pack()
        self.output_dir_button.pack()
        self.compile_btn.pack()
        self.input_fpath = tk.StringVar()
        self.input_fpath.set("No file selected")
        self.input_fpath_label = tk.Label()
        self.input_fpath_label["textvariable"] = self.input_fpath
        self.output_fpath_label = tk.Label()
        self.out_dir = tk.StringVar()
        self.out_dir.set("No directory selected")
        self.output_fpath_label["textvariable"] = self.out_dir
        self.html_style = ""

        self.input_fpath_label.place(x=150, y=3)
        self.output_fpath_label.place(x=150, y=33)
        tk.Radiobutton(self, text="Fancy", variable=self.var,
                       value=1, command=self.select_style).pack()
        tk.Radiobutton(self, text="Basic", variable=self.var,
                       value=2, command=self.select_style).pack()

    def open_file(self, dest):
        file = fd.askopenfile(filetypes=[('epicR Files', '*.rcp')])

        if file:
            filepath = os.path.abspath(file.name)
            dest.set(self.format_fpath(filepath))

    def select_style(self):
        value = self.var.get()

        if(value == 1):
            self.html_style = "Fancy"
        else:
            self.html_style = "Basic"

    def open_dir(self, dest):
        _dir = fd.askdirectory()

        if _dir:
            dest.set(self.format_fpath(_dir))

    def compile(self):
        executable = os.path.join(".", "bin", "main")
        clargs = ("%s -o %s --%s" %
                  (self.input_fpath.get(), self.out_dir.get(), self.html_style))

        os.system("%s %s" % (
            executable,
            clargs
        ))

    def format_fpath(self, s):
        return s


root = tk.Tk()
app = App(root)
app.mainloop()
