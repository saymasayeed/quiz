import tkinter as tk
from tkinter import ttk
import mysql.connector
import re
from tkinter import messagebox
from tabulate import tabulate

db_config = {
    "host": "localhost",
    "user": "root",
    "password": "12345",
    "database": "login"
}

class Course:
    def __init__(self, course_id, name, schedule, capacity):
        self.course_id = course_id
        self.name = name
        self.schedule = schedule
        self.capacity = capacity

class Room:
    def __init__(self, room_no, capacity):
        self.room_no = room_no
        self.capacity = capacity

class RoomAllocation:
    def __init__(self, allocation_id, room, from_time, to_time, day, course):
        self.allocation_id = allocation_id
        self.room = room
        self.from_time = from_time
        self.to_time = to_time
        self.day = day
        self.course = course

class CourseSchedulerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Course Scheduling")
        self.root.configure(bg='#050533')

        self.create_widgets()
        self.output_popup = None  # Initialize a variable to store the output popup window

    def create_widgets(self):
        notebook = ttk.Notebook(self.root)
        notebook.pack()

        output_frame = tk.Frame(notebook, bg='#050533')
        output_frame.pack(fill="both", expand=True)
        notebook.add(output_frame, text='Output', padding=10)

        self.output_text = tk.Text(output_frame, state=tk.DISABLED, wrap=tk.WORD, height=20, width=50, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        self.output_text.pack()

        course_frame = tk.Frame(notebook, bg='#050533')
        course_frame.pack(fill="both", expand=True)
        notebook.add(course_frame, text='Course', padding=10)

        course_id_label = tk.Label(course_frame, text="Course ID:", bg='#050533', fg='#F2F1E8')
        course_id_label.pack()
        self.course_id_entry = tk.Entry(course_frame)
        self.course_id_entry.pack()

        course_name_label = tk.Label(course_frame, text="Course Name:", bg='#050533', fg='#F2F1E8')
        course_name_label.pack()
        self.course_name_entry = tk.Entry(course_frame)
        self.course_name_entry.pack()

        time_label = tk.Label(course_frame, text="Time:", bg='#050533', fg='#F2F1E8')
        time_label.pack()
        self.time_entry = tk.Entry(course_frame)
        self.time_entry.pack()

        enroll_label = tk.Label(course_frame, text="Enrollment Capacity:", bg='#050533', fg='#F2F1E8')
        enroll_label.pack()
        self.enroll_entry = tk.Entry(course_frame)
        self.enroll_entry.pack()

        preference_label = tk.Label(course_frame, text="Preference:", bg='#050533', fg='#F2F1E8')
        preference_label.pack()
        self.preference_entry = tk.Entry(course_frame)
        self.preference_entry.pack()

        update_course_button = tk.Button(course_frame, text="Update Course", command=self.update_course, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        update_course_button.pack()

        room_frame = tk.Frame(notebook, bg='#050533')
        room_frame.pack(fill="both", expand=True)
        notebook.add(room_frame, text='Room', padding=10)

        room_no_label = tk.Label(room_frame, text="Room Number:", bg='#050533', fg='#F2F1E8')
        room_no_label.pack()
        self.room_no_entry = tk.Entry(room_frame)
        self.room_no_entry.pack()

        capacity_label = tk.Label(room_frame, text="Capacity:", bg='#050533', fg='#F2F1E8')
        capacity_label.pack()
        self.capacity_entry = tk.Entry(room_frame)
        self.capacity_entry.pack()

        update_room_button = tk.Button(room_frame, text="Update Room", command=self.update_room, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        update_room_button.pack()

        room_view_button = tk.Button(room_frame, text="View Rooms Table", command=self.view_rooms_table, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        room_view_button.pack()

        course_view_button = tk.Button(course_frame, text="View Course Table", command=self.view_course_table, bg='#E34234', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        course_view_button.pack()

        schedule_courses_button = tk.Button(course_frame, text="Schedule Courses", command=self.show_schedule_output, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        schedule_courses_button.pack()

        refresh_button = tk.Button(course_frame, text="Refresh Output", command=self.refresh_output, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        refresh_button.pack()

    def refresh_output(self):
        self.output_text.config(state=tk.NORMAL)
        self.output_text.delete(1.0, tk.END)
        self.output_text.config(state=tk.DISABLED)

    def update_course(self):
        course_id = self.course_id_entry.get()
        course_name = self.course_name_entry.get()
        time = self.time_entry.get()
        enroll = self.enroll_entry.get()
        preference = self.preference_entry.get()

        try:
            conn = mysql.connector.connect(**db_config)
            cursor = conn.cursor()
            cursor.execute(f"INSERT INTO course (course_id, course_name, time, enroll, preference) VALUES ({course_id}, '{course_name}', '{time}', {enroll}, '{preference}')")
            conn.commit()
            conn.close()
            self.output_text.config(state=tk.NORMAL)
            self.output_text.insert(tk.END, "Course updated successfully.\n")
            self.output_text.config(state=tk.DISABLED)
            messagebox.showinfo("Success", "Course updated successfully.")
        except mysql.connector.Error as err:
            self.output_text.config(state=tk.NORMAL)
            self.output_text.insert(tk.END, f"Error updating course: {err}\n")
            self.output_text.config(state=tk.DISABLED)

    def update_room(self):
        room_no = self.room_no_entry.get()
        capacity = self.capacity_entry.get()

        try:
            conn = mysql.connector.connect(**db_config)
            cursor = conn.cursor()
            cursor.execute(f"INSERT INTO rooms (room_no, capacity) VALUES ({room_no}, {capacity})")
            conn.commit()
            conn.close()
            self.output_text.config(state=tk.NORMAL)
            self.output_text.insert(tk.END, "Room updated successfully.\n")
            self.output_text.config(state=tk.DISABLED)
            messagebox.showinfo("Success", "Room updated successfully.")
        except mysql.connector.Error as err:
            self.output_text.config(state=tk.NORMAL)
            self.output_text.insert(tk.END, f"Error updating room: {err}\n")
            self.output_text.config(state=tk.DISABLED)

    def view_course_table(self):
        course_window = tk.Toplevel(self.root)
        course_window.title("Course Table")
        course_window.geometry("600x400")  # Increase the window size

        course_text = tk.Text(course_window, wrap=tk.WORD, height=20, width=50, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        course_text.pack()

        try:
            conn = mysql.connector.connect(**db_config)
            cursor = conn.cursor()
            cursor.execute("SELECT * FROM course")
            course_db = cursor.fetchall()
            conn.close()
        except mysql.connector.Error as err:
            course_text.insert(tk.END, f"Error fetching course data: {err}")

        course_text.insert(tk.END, "course_id course_name time enroll preference\n")
        for row in course_db:
            course_text.insert(tk.END, f"{row[0]} | {row[1]} | {row[2]} | {row[3]} | {row[4]}\n")

    def view_rooms_table(self):
        rooms_window = tk.Toplevel(self.root)
        rooms_window.title("Rooms Table")
        rooms_window.geometry("600x400")  # Increase the window size

        rooms_text = tk.Text(rooms_window, wrap=tk.WORD, height=20, width=50, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        rooms_text.pack()

        try:
            conn = mysql.connector.connect(**db_config)
            cursor = conn.cursor()
            cursor.execute("SELECT * FROM rooms")
            rooms_db = cursor.fetchall()
            conn.close()
        except mysql.connector.Error as err:
            rooms_text.insert(tk.END, f"Error fetching rooms data: {err}")

        rooms_text.insert(tk.END, "room_no capacity\n")
        for row in rooms_db:
            rooms_text.insert(tk.END, f"{row[0]} | {row[1]}\n")

    def show_schedule_output(self):
        # Create a new popup window for displaying the output
        if self.output_popup is None or not self.output_popup.winfo_exists():
            self.output_popup = tk.Toplevel(self.root)
            self.output_popup.title("Course Schedule Output")
            self.output_popup.geometry("800x600")  # Adjust the size as needed

        # Create a Text widget in the popup window to display the output
        output_text = tk.Text(self.output_popup, wrap=tk.WORD, height=20, width=50, bg='#0D698B', fg='#F2F1E8', bd=2, relief=tk.SOLID)
        output_text.pack()

        # Call your schedule_courses method to generate and display the output
        self.schedule_courses(output_text)

    def schedule_courses(self, output_text):
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
                day_time = self.interpreter(j)
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

        values = []
        for i in final_sched:
            values.append(i.values())
        output_text.config(state=tk.NORMAL)
        mystr = tabulate(values, headers=times, tablefmt="grid")
        output_text.insert(tk.END, mystr + "\n")
        output_text.insert(tk.END, "course_id, room_no for Monday, Tuesday, Wednesday, Friday\n")
        output_text.config(state=tk.DISABLED)

    def interpreter(self, schedule):
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

if __name__ == "__main__":
    root = tk.Tk()
    app = CourseSchedulerApp(root)
    root.mainloop()

