#include <wx/wx.h>
#include <wx/listctrl.h>
#include <sqlite3.h>
#include <locale>
#include "tsnsoft.xpm" // Подключаем иконку

// Определение главного окна приложения
class MyFrame : public wxFrame {
public:
	MyFrame() : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("База данных книг"), wxDefaultPosition, wxSize(600, 350), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {
		Centre(); // Центрируем окно на экране
		SetIcon(wxIcon(tsnsoft_xpm)); // Устанавливаем иконку
		
		// Создаем список с колонками для отображения данных
		listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
		listCtrl->AppendColumn(wxString::FromUTF8("ID"), wxLIST_FORMAT_LEFT, 50);
		listCtrl->AppendColumn(wxString::FromUTF8("Автор"), wxLIST_FORMAT_LEFT, 200);
		listCtrl->AppendColumn(wxString::FromUTF8("Название книги"), wxLIST_FORMAT_LEFT, 250);
		listCtrl->AppendColumn(wxString::FromUTF8("Год"), wxLIST_FORMAT_LEFT, 80);
		
		InitDatabase(); // Инициализация базы данных
		LoadData(); // Загрузка данных в список
	}
	
	~MyFrame() {
		sqlite3_close(db); // Закрытие соединения с базой данных при выходе
	}
	
private:
	wxListCtrl* listCtrl; // Компонент списка
	sqlite3* db = nullptr; // Указатель на базу данных SQLite
	
	// Функция инициализации базы данных
	void InitDatabase() {
		sqlite3_open("books.db", &db); // Открываем (или создаем) базу данных
		
		// Создаем таблицу, если она не существует
		sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS books(id INTEGER PRIMARY KEY, author TEXT, title TEXT, year INTEGER);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, "DELETE FROM books;", nullptr, nullptr, nullptr); // Очищаем данные перед вставкой
		
		// Структура для хранения информации о книгах
		struct Book {
			std::string title;
			std::string author;
			int year;
		};
		
		// Список книг для заполнения базы данных
		std::vector<Book> books = {
			{"Эффективный C++", "С. Майерс", 1992},
			{"Философия C++", "Б. Эккель", 1998},
			{"Глубокое изучение C++", "А. Александреску", 2001},
			{"Алгоритмы на C++", "Р. Седжвик", 2001},
			{"Метапрограммирование на C++", "Д. Абрамс", 2004},
			{"Совершенный C++", "С. Майерс", 2005},
			{"Параллельное программирование на C++", "Э. Ладс", 2012},
			{"Язык программирования C++", "Б. Страуструп", 2013},
			{"C++ для начинающих", "Г. Шилдт", 2014},
			{"C++ без страха", "Б. Овертон", 2015},
			{"C++ стандартная библиотека", "Н. Йосипович", 2016},
			{"C++ в примерах", "С. Прата", 2018}
		};
		
		// Вставка данных в базу
		int id = 1;
		for (const auto& book : books) {
			std::string sql = "INSERT INTO books(id, author, title, year) VALUES(" + std::to_string(id) + ", '" + book.author + "', '" + book.title + "', " + std::to_string(book.year) + ");";
			sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
			id++;
		}
	}
	
	// Функция загрузки данных из базы в список
	void LoadData() {
		listCtrl->DeleteAllItems(); // Очищаем список перед загрузкой данных
		sqlite3_stmt* stmt;
		
		// Выполняем SQL-запрос с сортировкой по году
		sqlite3_prepare_v2(db, "SELECT * FROM books ORDER BY year ASC", -1, &stmt, nullptr);
		
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			long index = listCtrl->InsertItem(listCtrl->GetItemCount(), wxString::Format("%d", sqlite3_column_int(stmt, 0))); // ID
			listCtrl->SetItem(index, 1, wxString::FromUTF8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))); // Название
			listCtrl->SetItem(index, 2, wxString::FromUTF8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))); // Автор
			listCtrl->SetItem(index, 3, wxString::Format("%d", sqlite3_column_int(stmt, 3))); // Год
		}
		sqlite3_finalize(stmt); // Освобождаем ресурсы
	}
};

// Определение основного приложения wxWidgets
class MyApp : public wxApp {
public:
	virtual bool OnInit() {
		setlocale(LC_ALL, ""); // Устанавливаем поддержку русских символов
		MyFrame* frame = new MyFrame(); // Создаем главное окно
		frame->Show(true); // Показываем окно
		return true;
	}
};

wxIMPLEMENT_APP(MyApp); // Точка входа в приложение wxWidgets

