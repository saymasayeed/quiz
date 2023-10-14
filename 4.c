import tkinter as tk
from tkinter import messagebox
import mysql.connector
import subprocess

# Function to enable input fields and buttons
def enable_input_fields():
    entry_fullname.config(state="normal")
    entry_coursename.config(state="normal")
    entry_password.config(state="normal")
    entry_timeslot.config(state="normal")
    entry_gender_male.config(state="normal")
    entry_gender_female.config(state="normal")
    entry_gender_not_say.config(state="normal")
    update_button.config(state="normal")

# Function to disable input fields and buttons
def disable_input_fields():
    entry_fullname.config(state="disabled")
    entry_coursename.config(state="disabled")
    entry_password.config(state="disabled")
    entry_timeslot.config(state="disabled")
    entry_gender_male.config(state="disabled")
    entry_gender_female.config(state="disabled")
    entry_gender_not_say.config(state="disabled")
    update_button.config(state="disabled")

# Function to clear the side pane
def clear_side_pane():
    side_pane.delete(0, tk.END)

# Function to search for a user by username and display info in the side pane
def search_user():
    username = entry_username.get()

    clear_side_pane()  # Clear side pane before displaying new data
    disable_input_fields()  # Disable input fields and buttons

    # Connect to the MySQL database
    connection = mysql.connector.connect(
        host='localhost',
        user='root',
        password='12345',
        database='login'
    )

    cursor = connection.cursor()

    # Execute a SELECT query to retrieve user information
    query = "SELECT * FROM users WHERE fullname = %s"
    cursor.execute(query, (username,))
    user = cursor.fetchone()

    if user:
        # Display the user information in the side pane
        side_pane.insert(tk.END, f"Full Name: {user[0]}")
        side_pane.insert(tk.END, f"Course Name: {user[1]}")
        side_pane.insert(tk.END, f"Password: {user[2]}")
        side_pane.insert(tk.END, f"Time Slot: {user[3]}")
        side_pane.insert(tk.END, "Gender:")
        gender_value = user[4]
        if gender_value == "Male":
            side_pane.insert(tk.END, "  - Male")
        elif gender_value == "Female":
            side_pane.insert(tk.END, "  - Female")
        else:
            side_pane.insert(tk.END, "  - Rather not say")
        
        enable_input_fields()  # Enable input fields and buttons
        
    else:
        messagebox.showerror("Error", "User not found")

    cursor.close()
    connection.close()

# Function to update user information
def update_user():
    username = entry_username.get()
    new_fullname = entry_fullname.get()
    new_coursename = entry_coursename.get()
    new_password = entry_password.get()
    new_timeslot = entry_timeslot.get()
    new_gender = gender_var.get()

    # Connect to the MySQL database
    connection = mysql.connector.connect(
        host='localhost',
        user='root',
        password='12345',
        database='login'
    )

    cursor = connection.cursor()

    # Execute an UPDATE query to modify user information
    query = "UPDATE users SET fullname = %s, coursename = %s, password = %s, timeslot = %s, gender = %s WHERE fullname = %s"
    cursor.execute(query, (new_fullname, new_coursename, new_password, new_timeslot, new_gender, username))
    connection.commit()

    cursor.close()
    connection.close()

    messagebox.showinfo("Success", "User information updated successfully")

# Function to open the schedule Python file
def open_schedule_file():
    subprocess.Popen(["python", "s1.py"])

# Create the tkinter window
root = tk.Tk()
root.title("User Information")

# Set the background color
root.configure(bg="#050533")

# Create tkinter variables to store user information
fullname_var = tk.StringVar()
coursename_var = tk.StringVar()
password_var = tk.StringVar()
timeslot_var = tk.StringVar()
gender_var = tk.StringVar(value="Male")  # Default gender value

# Labels and Entry widgets
label_username = tk.Label(root, text="Enter Username:", bg="#050533", fg="#F2F1E8")
entry_username = tk.Entry(root, bg="#F2F1E8")
label_fullname = tk.Label(root, text="Full Name:", bg="#050533", fg="#F2F1E8")
entry_fullname = tk.Entry(root, bg="#F2F1E8", textvariable=fullname_var, state="disabled", disabledbackground="gray")
label_coursename = tk.Label(root, text="Course Name:", bg="#050533", fg="#F2F1E8")
entry_coursename = tk.Entry(root, bg="#F2F1E8", textvariable=coursename_var, state="disabled", disabledbackground="gray")
label_password = tk.Label(root, text="Password:", bg="#050533", fg="#F2F1E8")
entry_password = tk.Entry(root, bg="#F2F1E8", textvariable=password_var, state="disabled", disabledbackground="gray")
label_timeslot = tk.Label(root, text="Time Slot:", bg="#050533", fg="#F2F1E8")
entry_timeslot = tk.Entry(root, bg="#F2F1E8", textvariable=timeslot_var, state="disabled", disabledbackground="gray")
label_gender = tk.Label(root, text="Gender:", bg="#050533", fg="#F2F1E8")
entry_gender_male = tk.Radiobutton(root, text="Male", variable=gender_var, value="Male", bg="#050533", fg="#F2F1E8", selectcolor="#050533", state="disabled")
entry_gender_female = tk.Radiobutton(root, text="Female", variable=gender_var, value="Female", bg="#050533", fg="#F2F1E8", selectcolor="#050533", state="disabled")
entry_gender_not_say = tk.Radiobutton(root, text="Rather not say", variable=gender_var, value="Rather not say", bg="#050533", fg="#F2F1E8", selectcolor="#050533", state="disabled")

# Side Pane to display existing user information
side_pane = tk.Listbox(root, bg="#E34234", width=30, fg="#F2F1E8")
side_pane.grid(row=0, column=3, rowspan=8, sticky="ns")

# Buttons
search_button = tk.Button(root, text="Search", bg="#0D698B", fg="#F2F1E8", command=search_user)
update_button = tk.Button(root, text="Update Info", bg="#0D698B", fg="#F2F1E8", command=update_user, state="disabled")
schedule_button = tk.Button(root, text="Schedule", bg="#0D698B", fg="#F2F1E8", command=open_schedule_file)

# Grid layout
label_username.grid(row=0, column=0, padx=10, pady=5)
entry_username.grid(row=0, column=1, padx=10, pady=5)
search_button.grid(row=0, column=2, padx=10, pady=5)

label_fullname.grid(row=1, column=0, padx=10, pady=5)
entry_fullname.grid(row=1, column=1, padx=10, pady=5)

label_coursename.grid(row=2, column=0, padx=10, pady=5)
entry_coursename.grid(row=2, column=1, padx=10, pady=5)

label_password.grid(row=3, column=0, padx=10, pady=5)
entry_password.grid(row=3, column=1, padx=10, pady=5)

label_timeslot.grid(row=4, column=0, padx=10, pady=5)
entry_timeslot.grid(row=4, column=1, padx=10, pady=5)

label_gender.grid(row=5, column=0, padx=10, pady=5)
entry_gender_male.grid(row=5, column=1, padx=10, pady=5)
entry_gender_female.grid(row=6, column=1, padx=10, pady=5)
entry_gender_not_say.grid(row=7, column=1, padx=10, pady=5)

update_button.grid(row=8, column=1, padx=10, pady=10)
schedule_button.grid(row=9, column=1, padx=10, pady=10)

root.mainloop()

