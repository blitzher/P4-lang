import os
from tkinter import *
from tkinter import ttk
from tkinter.filedialog import *
root = Tk()
frm = root.geometry("500x300")
root.resizable()
filepath = ""


def open_file():
    global filepath
    file = askopenfile(filetypes=[('epicR Files', '*.rcp')])
    if file:
        filepath = os.path.abspath(file.name)


ttk.Button(frm, text="Browse", command=open_file).grid(column=1, row=0)

root.mainloop()
