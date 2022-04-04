from http.server import executable
import os
import tkinter as tk
from tkinter import ttk
import tkinter.filedialog as fd


class App(tk.Frame):

    def __init__(self, master):
        super().__init__(master)
        root.geometry("550x100")
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
        self.html_style = "basic"
        # todo: add radio buttons to change

        self.input_fpath_label.place(x=150, y=3)
        self.output_fpath_label.place(x=150, y=33)

    def open_file(self, dest):
        file = fd.askopenfile(filetypes=[('epicR Files', '*.rcp')])

        if file:
            filepath = os.path.abspath(file.name)
            dest.set(self.format_fpath(filepath))

    def open_dir(self, dest):
        _dir = fd.askdirectory()

        if _dir:
            dest.set(self.format_fpath(_dir))

    def compile(self):
        executable = os.path.join(".", "bin", "main")
        clargs = ("%s -o %s --%s" %
                  (self.input_fpath, self.out_dir, self.html_style))

        os.system("%s %s" % (
            executable,
            clargs
        ))

    def format_fpath(self, s):
        max_len = 50
        if (len(s) < max_len):
            return s
        else:
            return '...' + s[-max_len:-1] + s[-1]


root = tk.Tk()
app = App(root)
app.mainloop()
