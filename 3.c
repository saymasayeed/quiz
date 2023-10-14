import tkinter as tk
from tkinter import ttk
import mysql.connector
import re
from tabulate import tabulate
from tkinter import messagebox

db_config = {
    "host": "localhost",
    "user": "root",
    "password": "12345",
    "database": "login"
}

def refresh_output():
    output_text.config(state=tk.NORMAL)
    output_text.delete(1.0, tk.END)
    output_text.config(state=tk.DISABLED)

def schedule_courses():
    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
    except mysql.connector.Error as err:
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Error connecting to database: {err}\n")
        output_text.config(state=tk.DISABLED)
        return

    cursor.execute("SELECT * FROM course")
    course_db = cursor.fetchall()

    for i in course_db:
        for j in i[-1].split(","):
            day_time = interpreter(j)
            for k in range(len(day_time[0])):
                cursor.execute(f"select room_no from room_allos where day='{day_time[0][k]}' and from_time<='{day_time[1][k]}' and to_time>='{day_time[1][k]}';")
                conflicts = [i[0] for i in cursor.fetchall()]
                cursor.execute(f"select room_no from rooms where capacity>='{i[3]}';")
                room_poss = [i[0] for i in cursor.fetchall()]
                cursor.execute("select * from room_allos")
                room_allos = (cursor.fetchall())
                if len(conflicts) == 0:
                    output_text.config(state=tk.NORMAL)
                    output_text.insert(tk.END, "No Conflicts\n")
                    output_text.config(state=tk.DISABLED)
                    cursor.execute(f"insert into room_allos values('{len(room_allos) + 1}', '{room_poss[0]}', '{day_time[1][k]}', '{day_time[1][k] + 100}', '{day_time[0][k]}', '{i[0]}')")
                else:
                    output_text.config(state=tk.NORMAL)
                    output_text.insert(tk.END, "Conflicts\n")
                    output_text.config(state=tk.DISABLED)
                    for con in conflicts:
                        try:
                            room_poss.remove(con)
                        except Exception as e:
                            pass

                    if len(room_poss) > 0:
                        cursor.execute(
                            f"insert into room_allos values('{len(room_allos) + 1}', '{room_poss[0]}', '{day_time[1][k]}', '{day_time[1][k] + 100}', '{day_time[0][k]}', '{i[0]}')")
                    else:
                        output_text.config(state=tk.NORMAL)
                        output_text.insert(tk.END, "No Rooms\n")
                        output_text.config(state=tk.DISABLED)
                        break

    cursor.execute("select * from room_allos")
    test = cursor.fetchall()
    # print("id", "room_no", "from_time", "to_time", "day", "course_id")
    final_sched = []
    days = ["Monday", "Tuesday", "Wednesday", "Friday"]

    times = ["9:00-10:00", "10:00-11:00", "11:00-12:00"]


    for j in days:
        temp = dict()
        for i in test:
            if i[-2] == j and i[-1] not in [k[0] for k in list(temp.values())]:
                temp[i[2:4]] = [i[-1], i[1]]
        final_sched.append(temp)

    for sched in range(len(final_sched)):
        myKeys = list(final_sched[sched].keys())
        myKeys.sort()
        final_sched[sched] = {i: final_sched[sched][i] for i in myKeys}

    for i in final_sched:
        print(i)

    values = []
    for i in final_sched:
        values.append(i.values())
    output_text.config(state=tk.NORMAL)
    #print(tabulate(values, headers=times, tablefmt="grid"))
    mystr=tabulate(values, headers=times, tablefmt="grid")
    output_text.insert(tk.END, mystr+"\n")
    output_text.insert(tk.END, "course_id, room_no for Monday, Tuesday, Wednesday, Friday\n")
    #print("course_id, room_no for Monday, Tuesday, Wednesday, Friday")
    
    
    output_text.config(state=tk.DISABLED)

def interpreter(schedule):
    day_map = {
        'M': "Monday",
        'TT': "Tuesday",
        'W': "Wednesday",
        'F': "Friday",
    }
    day_time = [[], []]

    for i in day_map:
        if i in schedule:
            day_time[0].append(day_map[i])
            match = re.search(r'\d{1,2}:\d{2}', schedule)

            if match:
                time = match.group().replace(":", "")
                day_time[1].append(int(time))
            else:
                match_hours = re.search(r'\d{1,2}', schedule)
                if match_hours:
                    hours = match_hours.group()
                    time = hours + "00"
                    day_time[1].append(int(time))

    return day_time

def update_course():
    course_id = course_id_entry.get()
    course_name = course_name_entry.get()
    time = time_entry.get()
    enroll = enroll_entry.get()
    preference = preference_entry.get()

    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute(f"INSERT INTO course (course_id, course_name, time, enroll, preference) VALUES ({course_id}, '{course_name}', '{time}', {enroll}, '{preference}')")
        conn.commit()
        conn.close()
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, "Course updated successfully.\n")
        output_text.config(state=tk.DISABLED)
        messagebox.showinfo("Success", "Course updated successfully.")
    except mysql.connector.Error as err:
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Error updating course: {err}\n")
        output_text.config(state=tk.DISABLED)

def update_room():
    room_no = room_no_entry.get()
    capacity = capacity_entry.get()

    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute(f"INSERT INTO rooms (room_no, capacity) VALUES ({room_no}, {capacity})")
        conn.commit()
        conn.close()
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, "Room updated successfully.\n")
        output_text.config(state=tk.DISABLED)
        messagebox.showinfo("Success", "Room updated successfully.")
    except mysql.connector.Error as err:
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Error updating room: {err}\n")
        output_text.config(state=tk.DISABLED)

def view_course_table():
    course_window = tk.Toplevel(course_frame)
    course_window.title("Course Table")
    course_window.geometry("600x400")  # Set a larger window size

    course_text = tk.Text(course_window, wrap=tk.WORD, height=20, width=50)
    course_text.pack()

    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute("SELECT * FROM course")
        course_db = cursor.fetchall()

        course_text.insert(tk.END, "Course ID | Course Name | Time | Enroll | Preference\n")
        for row in course_db:
            course_text.insert(tk.END, f"{row[0]} | {row[1]} | {row[2]} | {row[3]} | {row[4]}\n")

        conn.close()
    except mysql.connector.Error as err:
        course_text.insert(tk.END, f"Error fetching course data: {err}")

def view_rooms_table():
    rooms_window = tk.Toplevel(room_frame)
    rooms_window.title("Rooms Table")
    rooms_window.geometry("600x400")  # Set a larger window size

    rooms_text = tk.Text(rooms_window, wrap=tk.WORD, height=20, width=50)
    rooms_text.pack()

    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute("SELECT * FROM rooms")
        rooms_db = cursor.fetchall()

        rooms_text.insert(tk.END, "Room Number | Capacity\n")
        for row in rooms_db:
            rooms_text.insert(tk.END, f"{row[0]} | {row[1]}\n")

        conn.close()
    except mysql.connector.Error as err:
        rooms_text.insert(tk.END, f"Error fetching rooms data: {err}")

def remove_course():
    course_id = course_id_entry.get()
    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute(f"DELETE FROM course WHERE course_id={course_id}")
        conn.commit()
        conn.close()
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Course with course_id {course_id} removed successfully.\n")
        output_text.config(state=tk.DISABLED)
        messagebox.showinfo("Success", f"Course with course_id {course_id} removed successfully.")
    except mysql.connector.Error as err:
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Error removing course: {err}\n")
        output_text.config(state=tk.DISABLED)

def remove_room():
    room_no = room_no_entry.get()
    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        cursor.execute(f"DELETE FROM rooms WHERE room_no={room_no}")
        conn.commit()
        conn.close()
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Room with room_no {room_no} removed successfully.\n")
        output_text.config(state=tk.DISABLED)
        messagebox.showinfo("Success", f"Room with room_no {room_no} removed successfully.")
    except mysql.connector.Error as err:
        output_text.config(state=tk.NORMAL)
        output_text.insert(tk.END, f"Error removing room: {err}\n")
        output_text.config(state=tk.DISABLED)

root = tk.Tk()
root.title("Course Scheduling")
root.configure(bg='#050533')

notebook = ttk.Notebook(root)
notebook.pack()

output_text = tk.Text(root, state=tk.DISABLED, wrap=tk.WORD, height=20, width=50, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
output_text.pack()

course_frame = tk.Frame(notebook, bg='#050533')
course_frame.pack(fill="both", expand=True)
notebook.add(course_frame, text='Course', padding=10)

course_id_label = tk.Label(course_frame, text="Course ID:", fg='#F2F1E8', bg='#050533')
course_id_label.pack()
course_id_entry = tk.Entry(course_frame, bg='#F2F1E8', fg='#050533')
course_id_entry.pack()

course_name_label = tk.Label(course_frame, text="Course Name:", fg='#F2F1E8', bg='#050533')
course_name_label.pack()
course_name_entry = tk.Entry(course_frame, bg='#F2F1E8', fg='#050533')
course_name_entry.pack()

time_label = tk.Label(course_frame, text="Time:", fg='#F2F1E8', bg='#050533')
time_label.pack()
time_entry = tk.Entry(course_frame, bg='#F2F1E8', fg='#050533')
time_entry.pack()

enroll_label = tk.Label(course_frame, text="Enroll:", fg='#F2F1E8', bg='#050533')
enroll_label.pack()
enroll_entry = tk.Entry(course_frame, bg='#F2F1E8', fg='#050533')
enroll_entry.pack()

preference_label = tk.Label(course_frame, text="Preference:", fg='#F2F1E8', bg='#050533')
preference_label.pack()
preference_entry = tk.Entry(course_frame, bg='#F2F1E8', fg='#050533')
preference_entry.pack()

course_update_button = tk.Button(course_frame, text="Update Course", command=update_course, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
course_update_button.pack()

room_frame = tk.Frame(notebook, bg='#050533')
room_frame.pack(fill="both", expand=True)
notebook.add(room_frame, text='Room', padding=10)

room_no_label = tk.Label(room_frame, text="Room Number:", fg='#F2F1E8', bg='#050533')
room_no_label.pack()
room_no_entry = tk.Entry(room_frame, bg='#F2F1E8', fg='#050533')
room_no_entry.pack()

capacity_label = tk.Label(room_frame, text="Capacity:", fg='#F2F1E8', bg='#050533')
capacity_label.pack()
capacity_entry = tk.Entry(room_frame, bg='#F2F1E8', fg='#050533')
capacity_entry.pack()

room_update_button = tk.Button(room_frame, text="Update Room", command=update_room, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
room_update_button.pack()

remove_course_button = tk.Button(course_frame, text="Remove Course by ID", command=remove_course, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
remove_course_button.pack()

remove_room_button = tk.Button(room_frame, text="Remove Room by Number", command=remove_room, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
remove_room_button.pack()

schedule_courses_button = tk.Button(root, text="Schedule Courses", command=schedule_courses, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
schedule_courses_button.pack()

refresh_button = tk.Button(root, text="Refresh", command=refresh_output, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
refresh_button.pack()

view_course_table_button = tk.Button(course_frame, text="View Course Table", command=view_course_table, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
view_course_table_button.pack()

view_rooms_table_button = tk.Button(room_frame, text="View Rooms Table", command=view_rooms_table, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
view_rooms_table_button.pack()

root.mainloop()

