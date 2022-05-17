import os
import tkinter as tk
from tkinter import E, LEFT, W, Entry, IntVar, ttk, messagebox
import tkinter.filedialog as fd


class App(tk.Frame):

    def __init__(self, master):
        super().__init__(master)
        root.geometry("600x210")
        root.title("epicR - A recipe specification language")
        self.html_buttons = IntVar()
        self.unit_buttons = IntVar()
        self.grid_configure(column=10, row=10)
        self.input_file_button = tk.Button(
            self,
            command=lambda: self.open_file(self.input_fpath),
            text="Choose file/Recipe",
            height=1,
            width=20,
        )
        self.output_dir_button = tk.Button(
            self,
            command=lambda: self.open_dir(self.out_dir_label),
            text="Choose output directory",
            height=1,
            width=20,
        )
        self.compile_btn = tk.Button(
            self,
            command=lambda: self.compile(),
            text="Compile output",
            height=1,
            width=20,
        )
        self.input_file_button.pack(anchor=W)
        self.output_dir_button.pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Fancy Output style",
            variable=self.html_buttons,
            value=1,
            command=self.select_style,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Basic Output style",
            variable=self.html_buttons,
            value=2,
            command=self.select_style,
        ).pack(anchor=W)

        separator = ttk.Separator(self, orient='horizontal')
        separator.pack(fill='x')

        tk.Radiobutton(
            self,
            text="Original",
            variable=self.unit_buttons,
            value=1,
            command=self.select_system,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Metric system",
            variable=self.unit_buttons,
            value=2,
            command=self.select_system,
        ).pack(anchor=W)
        tk.Radiobutton(
            self,
            text="Imperial system",
            variable=self.unit_buttons,
            value=3,
            command=self.select_system,
        ).pack(anchor=W)
        self.compile_btn.pack(anchor=W)
        self.input_fpath = tk.StringVar()
        self.input_fpath.set("No file or recipe selected")
        self.input_fpath_label = tk.Label()
        self.input_fpath_label["textvariable"] = self.input_fpath
        self.output_fpath_label = tk.Label()
        self.out_dir_label = tk.StringVar()
        self.out_dir_label.set("No directory selected")
        self.output_fpath_label["textvariable"] = self.out_dir_label
        self.html_style = "Fancy"
        self.unit_system = "None"
        self.directory = tk.StringVar()
        self.directory.set("")
        self.output_dir_clarg = tk.StringVar()
        self.output_dir_clarg.set("-o ")
        self.input_fpath_label.place(x=215, y=3)
        self.output_fpath_label.place(x=215, y=33)
        self.html_buttons.set(1)
        self.unit_buttons.set(1)

    def open_file(self, dest):
        file = fd.askopenfile(filetypes=[("epicR Files", "*.rcp")])

        if file:
            filepath = os.path.abspath(file.name)
            dest.set(self.format_fpath(filepath))

    def select_style(self):
        value = self.html_buttons.get()

        if value == 1:
            self.html_style = "Fancy"
        elif value == 2:
            self.html_style = "Basic"

    def select_system(self):
        value = self.unit_buttons.get()

        if value == 1:
            self.unit_system = "None"
        elif value == 2:
            self.unit_system = "Metric"
        elif value == 3:
            self.unit_system = "Imperial"

    def open_dir(self, dest):
        _dir = fd.askdirectory()

        if _dir:
            dirpath = os.path.abspath(_dir)
            dest.set(self.format_fpath(dirpath))
            self.directory.set(
                self.output_dir_clarg.get() + self.out_dir_label.get())

    def compile(self):

        if (os.name == "nt"):
            executable = os.path.join("bin", "main.exe")
        else:
            executable = os.path.join(".", "bin", "main")

        if(os.path.exists(".epicr-error.txt")):
            os.remove(".epicr-error.txt")

        clargs = "%s %s --%s --%s" % (
            self.input_fpath.get(),
            self.directory.get(),
            self.html_style,
            self.unit_system,
        )
        os.system("%s %s" % (executable, clargs))

        if(os.path.exists(".epicr-error.txt")):
            errorLog = open(".epicr-error.txt")
            messagebox.showerror(title="Error", message=errorLog.readline())
            errorLog.close()
            os.remove(".epicr-error.txt")
        else:
            messagebox.showinfo(
                title="Success", message="Compilation succeeded")

    def format_fpath(self, s):
        return s


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    app.mainloop()
