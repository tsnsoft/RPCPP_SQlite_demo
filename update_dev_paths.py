import tkinter as tk
from tkinter import filedialog, messagebox
import os
import json

# Путь к файлу конфигурации
CONFIG_FILE = 'update_dev_paths.json'

def browse_folder(initial_dir, title):
    """Функция для выбора папки с предложением начального пути"""
    folder_selected = filedialog.askdirectory(initialdir=initial_dir, title=title)
    return folder_selected

def browse_file(initial_file):
    """Функция для выбора файла .dev с начальным файлом"""
    initial_dir = os.path.dirname(initial_file) if initial_file and os.path.exists(initial_file) else "C:/"
    file_selected = filedialog.askopenfilename(
        initialdir=initial_dir,
        title="Выберите файл .dev",
        filetypes=(("Text files", "*.dev"),)
    )
    return file_selected

def load_config():
    """Загружает сохранённую конфигурацию, если она существует"""
    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, 'r') as config_file:
            return json.load(config_file)
    return {}

def save_config(wxwidgets_folder, dev_file):
    """Сохраняет текущие пути и последний файл .dev в конфигурационный файл"""
    config = {
        "wxwidgets_folder": wxwidgets_folder,
        "dev_file": dev_file
    }
    with open(CONFIG_FILE, 'w') as config_file:
        json.dump(config, config_file)

def update_dev_file(dev_file, wxwidgets_folder):
    """Функция для обновления пути в файле .dev"""
    try:
        with open(dev_file, 'r') as file:
            content = file.readlines()

        # Папки для включений и библиотек
        includes_line = "Includes ="
        libs_line = "Libs ="

        # Формируем пути с прямыми слэшами
        wx_include = wxwidgets_folder.replace('\\', '/') + '/include'
        wx_lib_mswud = wxwidgets_folder.replace('\\', '/') + '/lib/gcc_lib/mswud'
        wx_lib = wxwidgets_folder.replace('\\', '/') + '/lib/gcc_lib'

        # Обновляем строки с путями
        for i in range(len(content)):
            if includes_line in content[i]:
                content[i] = f"{includes_line} {wx_include};{wx_lib_mswud}\n"
            if libs_line in content[i]:
                content[i] = f"{libs_line} {wx_lib}\n"

        # Записываем изменённый файл
        with open(dev_file, 'w') as file:
            file.writelines(content)
        print("Файл успешно обновлён!")

    except Exception as e:
        print(f"Произошла ошибка: {e}")

def main():
    # Создаём графический интерфейс
    root = tk.Tk()
    root.withdraw()  # Скрываем основное окно

    # Загружаем предыдущие настройки, если они есть
    config = load_config()

    # Получаем сохранённые пути и файл
    wxwidgets_folder = config.get('wxwidgets_folder')
    dev_file = config.get('dev_file')

    # Проверяем, есть ли сохранённый путь wxWidgets и его существование
    use_saved_paths = False
    if wxwidgets_folder and os.path.exists(wxwidgets_folder):
        response = messagebox.askyesno(
            "Использовать сохранённые пути?",
            f"Найден сохранённый путь:\n\nwxWidgets:\n{wxwidgets_folder}\n\nИспользовать его?"
        )
        use_saved_paths = response

    # Если пользователь не хочет использовать сохранённый путь или его нет, запрашиваем новый
    if not use_saved_paths:
        print("Выберите каталог для wxWidgets.")
        wxwidgets_folder = browse_folder(
            initial_dir=wxwidgets_folder or "C:/",
            title="Выберите папку с wxWidgets"
        )
        if not wxwidgets_folder:
            print("Папка wxWidgets не выбрана. Программа завершена.")
            return

    # Выбор файла .dev для обновления
    print("Теперь выберите файл .dev для обновления:")
    dev_file = browse_file(dev_file)

    if not dev_file:
        print("Файл не выбран. Программа завершена.")
        return

    # Сохраняем выбранные пути и файл в конфигурационный файл
    save_config(wxwidgets_folder, dev_file)

    # Обновляем файл .dev с нужными путями
    update_dev_file(dev_file, wxwidgets_folder)

    # Показываем сообщение в окне о успешном выполнении
    messagebox.showinfo("Успех", "Все успешно выполнено!")

if __name__ == "__main__":
    main()