import os
import tkinter as tk
from tkinter import ttk
import tkinter.filedialog as fd
from turtle import color


class App(tk.Frame):

    def __init__(self, master):
        super().__init__(master)
        root.geometry("500x75")
        self.grid_configure(column=10, row=10)
        self.input_file_button = tk.Button(
            self, command=lambda: self.open_file(self.input_fpath), text="Open file", height=1, width=10)
        self.output_dir_button = tk.Button(
            self, command=lambda: self.open_dir(self.out_dir), text="Open output dir", height=1, width=10)
        self.input_file_button.pack()
        self.output_dir_button.pack()
        self.input_fpath = tk.StringVar()
        self.input_fpath.set("No file selected")
        self.input_fpath_label = tk.Label()
        self.input_fpath_label["textvariable"] = self.input_fpath
        self.output_fpath_label = tk.Label()
        self.out_dir = tk.StringVar()
        self.out_dir.set("No directory selected")
        self.output_fpath_label["textvariable"] = self.out_dir

        self.input_fpath_label.place(x=130, y=3)
        self.output_fpath_label.place(x=130, y=33)

    def open_file(self, dest):
        file = fd.askopenfile(filetypes=[('epicR Files', '*.rcp')])

        if file:
            filepath = os.path.abspath(file.name)
            dest.set(self.format_fpath(filepath))

    def open_dir(self, dest):
        _dir = fd.askdirectory()

        if _dir:
            dest.set(self.format_fpath(_dir))

    def format_fpath(self, s):
        max_len = 50
        if (len(s) < max_len):
            return s
        else:
            return '...' + s[-max_len:-1] + s[-1]


root = tk.Tk()
app = App(root)
app.mainloop()
"""
# import tkinter module
from tkinter import *
from tkinter.ttk import *

# creating main tkinter window/toplevel
master = Tk()

# this wil create a label widget
l1 = Label(master, text="First:")
l2 = Label(master, text="Second:")

# grid method to arrange labels in respective
# rows and columns as specified
l1.grid(row=0, column=0, sticky=W, pady=2)
l2.grid(row=1, column=0, sticky=W, pady=2)

# entry widgets, used to take entry from user
e1 = Entry(master)
e2 = Entry(master)

# this will arrange entry widgets
e1.grid(row=0, column=1, pady=2)
e2.grid(row=1, column=1, pady=2)

# infinite loop which can be terminated by keyboard
# or mouse interrupt
mainloop()
"""
