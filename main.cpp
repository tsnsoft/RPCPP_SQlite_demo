#include <wx/wx.h>
#include <wx/listctrl.h>
#include <sqlite3.h>
#include <locale>
#include <thread>
#include <vector>
#include "tsnsoft.xpm" // Подключаем иконку

// Определение события для передачи данных из потока в основной поток
wxDEFINE_EVENT(EVT_LOAD_DATA, wxThreadEvent);

// Определение главного окна приложения
class MyFrame : public wxFrame {
public:
	// Конструктор главного окна
	MyFrame() : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("База данных книг"), wxDefaultPosition, wxSize(600, 350), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {
		Centre(); // Центрируем окно на экране
		SetIcon(wxIcon(tsnsoft_xpm)); // Устанавливаем иконку

		// Создаем список с колонками для отображения данных
		listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
		listCtrl->AppendColumn(wxString::FromUTF8("ID"), wxLIST_FORMAT_LEFT, 50);
		listCtrl->AppendColumn(wxString::FromUTF8("Автор"), wxLIST_FORMAT_LEFT, 200);
		listCtrl->AppendColumn(wxString::FromUTF8("Название книги"), wxLIST_FORMAT_LEFT, 250);
		listCtrl->AppendColumn(wxString::FromUTF8("Год"), wxLIST_FORMAT_LEFT, 80);

		// Привязываем обработчик события загрузки данных
		Bind(EVT_LOAD_DATA, &MyFrame::OnLoadData, this);

		InitDatabase(); // Инициализация базы данных
		LoadDataAsync(); // Запуск загрузки данных в отдельном потоке
	}

	// Деструктор: закрываем соединение с базой данных
	~MyFrame() {
		sqlite3_close(db);
	}

private:
	wxListCtrl* listCtrl; // Элемент управления списком
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

		// Создаем список книг для заполнения базы данных
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

		// Вставляем книги в базу данных
		int id = 1;
		for (const auto& book : books) {
			std::string sql = "INSERT INTO books(id, author, title, year) VALUES(" + std::to_string(id) + ", '" + book.author + "', '" + book.title + "', " + std::to_string(book.year) + ");";
			sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
			id++;
		}
	}

	// Функция для загрузки данных в отдельном потоке
	void LoadDataAsync() {
		std::thread([this]() {
			wxThreadEvent event(EVT_LOAD_DATA);
			std::vector<std::tuple<int, std::string, std::string, int >> data;

			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "SELECT * FROM books ORDER BY year ASC", -1, &stmt, nullptr);

			// Читаем данные из базы
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				data.emplace_back(
				    sqlite3_column_int(stmt, 0),
				    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
				    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
				    sqlite3_column_int(stmt, 3)
				);
			}
			sqlite3_finalize(stmt);

			event.SetPayload(data);
			wxQueueEvent(this, event.Clone()); // Отправляем данные в основной поток
		}).detach();
	}

	// Обработчик события загрузки данных в GUI
	void OnLoadData(wxThreadEvent& event) {
		auto data = event.GetPayload<std::vector<std::tuple<int, std::string, std::string, int >>> ();
		listCtrl->DeleteAllItems();

		// Заполняем список книг в GUI
		for (const auto& [id, author, title, year] : data) {
			long index = listCtrl->InsertItem(listCtrl->GetItemCount(), wxString::Format("%d", id));
			listCtrl->SetItem(index, 1, wxString::FromUTF8(author));
			listCtrl->SetItem(index, 2, wxString::FromUTF8(title));
			listCtrl->SetItem(index, 3, wxString::Format("%d", year));
		}
	}
};

// Определение основного приложения wxWidgets
class MyApp :
	public wxApp {
public:
	virtual bool OnInit() {
		setlocale(LC_ALL, ""); // Устанавливаем поддержку русских символов
		MyFrame* frame = new MyFrame(); // Создаем главное окно
		frame->Show(true); // Показываем окно
		return true;
	}
};

wxIMPLEMENT_APP(MyApp); // Точка входа в приложение wxWidgets

