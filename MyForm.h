#pragma once

#include "API.h"
#include <msclr/marshal_cppstd.h>
#include "DataStorage.h"
#include <Windows.h>
#include <limits>
#include <vector>
#using <System.Windows.Forms.DataVisualization.dll>

namespace JPOproject {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace Runtime::InteropServices;
	using namespace System::Windows::Forms::DataVisualization::Charting;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	private:
		// Form components
		System::ComponentModel::Container^ components;
		AirQualityAPI* airQualityApi;
		System::Windows::Forms::ListView^ stationsListView;
		System::Windows::Forms::Button^ refreshButton;
		System::Windows::Forms::TextBox^ searchBox;
		System::Windows::Forms::Label^ statusLabel;
		System::Windows::Forms::Panel^ headerPanel;
		System::Windows::Forms::ListView^ sensorsListView;
		System::Windows::Forms::Label^ sensorsTitleLabel;
		System::Windows::Forms::Panel^ detailsPanel;
		System::Windows::Forms::Button^ backButton;

		// Komponenty do obs³ugi wykresów
		System::Windows::Forms::Panel^ chartPanel;
		System::Windows::Forms::DataVisualization::Charting::Chart^ measurementChart;
		System::Windows::Forms::Button^ showChartButton;
		System::Windows::Forms::Button^ analyzeDataButton;
		System::Windows::Forms::Label^ chartTitleLabel;
		System::Windows::Forms::ComboBox^ dateRangeComboBox;
		System::Windows::Forms::Button^ closeChartButton;
		System::Windows::Forms::Button^ saveButton;
		System::Windows::Forms::Button^ closeAppButton;

		// Zmienne do przechowywania aktualnie wybranego sensora
		int currentSensorId;
		String^ currentParamName;
		String^ currentParamFormula;

	public:
		MyForm(void)
		{
			components = gcnew System::ComponentModel::Container();
			InitializeComponent();

			try {
				airQualityApi = new AirQualityAPI();
				LoadStations();
				ConfigureChartInitialSettings();
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d inicjalizacji: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
			}
		}

	protected:
		~MyForm()
		{
			if (this->chartPanel != nullptr) {
				if (this->chartPanel->Controls->Contains(this->measurementChart)) {
					this->chartPanel->Controls->Remove(this->measurementChart);
				}
				if (this->Controls->Contains(this->chartPanel)) {
					this->Controls->Remove(this->chartPanel);
				}
			}

			if (this->measurementChart != nullptr) {
				this->measurementChart->Series->Clear();
				this->measurementChart->ChartAreas->Clear();
				this->measurementChart->Legends->Clear();
				delete this->measurementChart;
				this->measurementChart = nullptr;
			}

			if (airQualityApi != nullptr)
			{
				delete airQualityApi;
				airQualityApi = nullptr;
			}

			if (components)
			{
				delete components;
				components = nullptr;
			}
		}

	private:
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());

			// Inicjalizacja kontrolek
			this->stationsListView = (gcnew System::Windows::Forms::ListView());
			this->refreshButton = (gcnew System::Windows::Forms::Button());
			this->searchBox = (gcnew System::Windows::Forms::TextBox());
			this->statusLabel = (gcnew System::Windows::Forms::Label());
			this->headerPanel = (gcnew System::Windows::Forms::Panel());
			this->closeAppButton = (gcnew System::Windows::Forms::Button());
			this->detailsPanel = (gcnew System::Windows::Forms::Panel());
			this->showChartButton = (gcnew System::Windows::Forms::Button());
			this->backButton = (gcnew System::Windows::Forms::Button());
			this->sensorsTitleLabel = (gcnew System::Windows::Forms::Label());
			this->sensorsListView = (gcnew System::Windows::Forms::ListView());
			this->chartPanel = (gcnew System::Windows::Forms::Panel());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->closeChartButton = (gcnew System::Windows::Forms::Button());
			this->analyzeDataButton = (gcnew System::Windows::Forms::Button());
			this->dateRangeComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->chartTitleLabel = (gcnew System::Windows::Forms::Label());
			this->measurementChart = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());

			this->headerPanel->SuspendLayout();
			this->detailsPanel->SuspendLayout();
			this->chartPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->measurementChart))->BeginInit();
			this->SuspendLayout();

			// stationsListView
			this->stationsListView->FullRowSelect = true;
			this->stationsListView->GridLines = true;
			this->stationsListView->HideSelection = false;
			this->stationsListView->Location = System::Drawing::Point(72, 65);
			this->stationsListView->Name = L"stationsListView";
			this->stationsListView->Size = System::Drawing::Size(768, 502);
			this->stationsListView->TabIndex = 0;
			this->stationsListView->UseCompatibleStateImageBehavior = false;
			this->stationsListView->View = System::Windows::Forms::View::Details;
			this->stationsListView->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::stationsListView_SelectedIndexChanged);

			// refreshButton
			this->refreshButton->Location = System::Drawing::Point(740, 29);
			this->refreshButton->Name = L"refreshButton";
			this->refreshButton->Size = System::Drawing::Size(100, 30);
			this->refreshButton->TabIndex = 0;
			this->refreshButton->Text = L"Odœwie¿";
			this->refreshButton->UseVisualStyleBackColor = true;
			this->refreshButton->Click += gcnew System::EventHandler(this, &MyForm::refreshButton_Click);

			// searchBox
			this->searchBox->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Italic));
			this->searchBox->ForeColor = System::Drawing::Color::Gray;
			this->searchBox->Location = System::Drawing::Point(584, 29);
			this->searchBox->Name = L"searchBox";
			this->searchBox->Size = System::Drawing::Size(150, 30);
			this->searchBox->TabIndex = 1;
			this->searchBox->Text = L"Search station";
			this->searchBox->TextChanged += gcnew System::EventHandler(this, &MyForm::searchBox_TextChanged);
			this->searchBox->Enter += gcnew System::EventHandler(this, &MyForm::searchBox_Enter);
			this->searchBox->Leave += gcnew System::EventHandler(this, &MyForm::searchBox_Leave);

			// statusLabel
			this->statusLabel->BackColor = System::Drawing::Color::Transparent;
			this->statusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(238)));
			this->statusLabel->ForeColor = System::Drawing::Color::White;
			this->statusLabel->Location = System::Drawing::Point(70, 34);
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(496, 23);
			this->statusLabel->TabIndex = 2;
			this->statusLabel->Text = L"Ready";

			// closeAppButton
			this->closeAppButton->Location = System::Drawing::Point(791, 573);
			this->closeAppButton->Name = L"closeAppButton";
			this->closeAppButton->Size = System::Drawing::Size(132, 45);
			this->closeAppButton->TabIndex = 6;
			this->closeAppButton->Text = L"Zamknij";
			this->closeAppButton->UseVisualStyleBackColor = true;
			this->closeAppButton->Click += gcnew System::EventHandler(this, &MyForm::closeAppButton_Click);

			// headerPanel
			this->headerPanel->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"headerPanel.BackgroundImage")));
			this->headerPanel->Controls->Add(this->closeAppButton);
			this->headerPanel->Controls->Add(this->detailsPanel);
			this->headerPanel->Controls->Add(this->stationsListView);
			this->headerPanel->Controls->Add(this->refreshButton);
			this->headerPanel->Controls->Add(this->searchBox);
			this->headerPanel->Controls->Add(this->statusLabel);
			this->headerPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->headerPanel->Location = System::Drawing::Point(0, 0);
			this->headerPanel->Name = L"headerPanel";
			this->headerPanel->Size = System::Drawing::Size(934, 634);
			this->headerPanel->TabIndex = 1;

			// showChartButton
			this->showChartButton->Enabled = false;
			this->showChartButton->Location = System::Drawing::Point(590, 10);
			this->showChartButton->Name = L"showChartButton";
			this->showChartButton->Size = System::Drawing::Size(90, 25);
			this->showChartButton->TabIndex = 3;
			this->showChartButton->Text = L"Wykres";
			this->showChartButton->UseVisualStyleBackColor = true;
			this->showChartButton->Click += gcnew System::EventHandler(this, &MyForm::showChartButton_Click);

			// backButton
			this->backButton->Location = System::Drawing::Point(685, 10);
			this->backButton->Name = L"backButton";
			this->backButton->Size = System::Drawing::Size(80, 25);
			this->backButton->TabIndex = 2;
			this->backButton->Text = L"Powrót";
			this->backButton->UseVisualStyleBackColor = true;
			this->backButton->Click += gcnew System::EventHandler(this, &MyForm::backButton_Click);

			// sensorsTitleLabel
			this->sensorsTitleLabel->AutoSize = true;
			this->sensorsTitleLabel->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold));
			this->sensorsTitleLabel->ForeColor = System::Drawing::Color::White;
			this->sensorsTitleLabel->Location = System::Drawing::Point(3, 10);
			this->sensorsTitleLabel->Name = L"sensorsTitleLabel";
			this->sensorsTitleLabel->Size = System::Drawing::Size(242, 28);
			this->sensorsTitleLabel->TabIndex = 0;
			this->sensorsTitleLabel->Text = L"Stanowiska pomiarowe: ";

			// sensorsListView
			this->sensorsListView->BackColor = System::Drawing::Color::White;
			this->sensorsListView->FullRowSelect = true;
			this->sensorsListView->GridLines = true;
			this->sensorsListView->HideSelection = false;
			this->sensorsListView->Location = System::Drawing::Point(3, 40);
			this->sensorsListView->Name = L"sensorsListView";
			this->sensorsListView->Size = System::Drawing::Size(762, 459);
			this->sensorsListView->TabIndex = 1;
			this->sensorsListView->UseCompatibleStateImageBehavior = false;
			this->sensorsListView->View = System::Windows::Forms::View::Details;
			this->sensorsListView->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::sensorsListView_SelectedIndexChanged);

			// detailsPanel
			this->detailsPanel->BackColor = System::Drawing::Color::Transparent;
			this->detailsPanel->Controls->Add(this->showChartButton);
			this->detailsPanel->Controls->Add(this->backButton);
			this->detailsPanel->Controls->Add(this->sensorsTitleLabel);
			this->detailsPanel->Controls->Add(this->sensorsListView);
			this->detailsPanel->Location = System::Drawing::Point(72, 65);
			this->detailsPanel->Name = L"detailsPanel";
			this->detailsPanel->Size = System::Drawing::Size(768, 502);
			this->detailsPanel->TabIndex = 5;
			this->detailsPanel->Visible = false;

			// saveButton
			this->saveButton->Location = System::Drawing::Point(740, 29);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(98, 28);
			this->saveButton->TabIndex = 5;
			this->saveButton->Text = L"Zapisz";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &MyForm::saveButton_Click);

			// closeChartButton
			this->closeChartButton->Location = System::Drawing::Point(740, 65);
			this->closeChartButton->Name = L"closeChartButton";
			this->closeChartButton->Size = System::Drawing::Size(98, 28);
			this->closeChartButton->TabIndex = 4;
			this->closeChartButton->Text = L"Zamknij";
			this->closeChartButton->UseVisualStyleBackColor = true;
			this->closeChartButton->Click += gcnew System::EventHandler(this, &MyForm::closeChartButton_Click);

			// analyzeDataButton
			this->analyzeDataButton->Location = System::Drawing::Point(554, 29);
			this->analyzeDataButton->Name = L"analyzeDataButton";
			this->analyzeDataButton->Size = System::Drawing::Size(180, 28);
			this->analyzeDataButton->TabIndex = 3;
			this->analyzeDataButton->Text = L"Analizuj dane";
			this->analyzeDataButton->UseVisualStyleBackColor = true;
			this->analyzeDataButton->Click += gcnew System::EventHandler(this, &MyForm::analyzeDataButton_Click);

			// dateRangeComboBox
			this->dateRangeComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->dateRangeComboBox->FormattingEnabled = true;
			this->dateRangeComboBox->Location = System::Drawing::Point(554, 68);
			this->dateRangeComboBox->Name = L"dateRangeComboBox";
			this->dateRangeComboBox->Size = System::Drawing::Size(180, 24);
			this->dateRangeComboBox->TabIndex = 2;
			this->dateRangeComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::dateRangeComboBox_SelectedIndexChanged);

			// chartTitleLabel
			this->chartTitleLabel->AutoSize = true;
			this->chartTitleLabel->BackColor = System::Drawing::Color::Transparent;
			this->chartTitleLabel->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold));
			this->chartTitleLabel->ForeColor = System::Drawing::Color::White;
			this->chartTitleLabel->Location = System::Drawing::Point(70, 65);
			this->chartTitleLabel->Name = L"chartTitleLabel";
			this->chartTitleLabel->Size = System::Drawing::Size(173, 28);
			this->chartTitleLabel->TabIndex = 1;
			this->chartTitleLabel->Text = L"Dane pomiarowe";

			// measurementChart
			chartArea->Name = L"ChartArea1";
			this->measurementChart->ChartAreas->Add(chartArea);
			legend->Name = L"Legend1";
			this->measurementChart->Legends->Add(legend);
			this->measurementChart->Location = System::Drawing::Point(75, 105);
			this->measurementChart->Name = L"measurementChart";
			this->measurementChart->Size = System::Drawing::Size(768, 485);
			this->measurementChart->TabIndex = 0;
			this->measurementChart->Text = L"Dane pomiarowe";

			// chartPanel
			this->chartPanel->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"chartPanel.BackgroundImage")));
			this->chartPanel->Controls->Add(this->saveButton);
			this->chartPanel->Controls->Add(this->closeChartButton);
			this->chartPanel->Controls->Add(this->analyzeDataButton);
			this->chartPanel->Controls->Add(this->dateRangeComboBox);
			this->chartPanel->Controls->Add(this->chartTitleLabel);
			this->chartPanel->Controls->Add(this->measurementChart);
			this->chartPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->chartPanel->Location = System::Drawing::Point(0, 0);
			this->chartPanel->Name = L"chartPanel";
			this->chartPanel->Size = System::Drawing::Size(934, 634);
			this->chartPanel->TabIndex = 2;

			// MyForm
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(934, 634);
			this->Controls->Add(this->headerPanel);
			this->Controls->Add(this->chartPanel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Air Quality Monitor";

			this->headerPanel->ResumeLayout(false);
			this->headerPanel->PerformLayout();
			this->detailsPanel->ResumeLayout(false);
			this->detailsPanel->PerformLayout();
			this->chartPanel->ResumeLayout(false);
			this->chartPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->measurementChart))->EndInit();
			this->ResumeLayout(false);
		}

		// Funkcja konfiguruj¹ca pocz¹tkowe ustawienia wykresu
		void ConfigureChartInitialSettings() {
			dateRangeComboBox->Items->AddRange(gcnew cli::array< System::Object^ >(4) {
				L"Ostatnie 24 godziny", L"Ostatnie 3 dni", L"Ostatni tydzieñ", L"Wszystkie dostêpne dane"
			});
			dateRangeComboBox->SelectedIndex = 0;

			measurementChart->ChartAreas[0]->AxisX->LabelStyle->Format = "dd.MM HH:mm";
			measurementChart->ChartAreas[0]->AxisX->IntervalType = DateTimeIntervalType::Hours;
			measurementChart->ChartAreas[0]->AxisX->Interval = 6;
			measurementChart->ChartAreas[0]->AxisX->Title = "Data i czas";

			showChartButton->Enabled = false;
		}

		// Funkcja do wyœwietlania danych pomiarowych na wykresie
		void ShowMeasurementChart(int sensorId, String^ paramName, String^ paramFormula) {
			currentSensorId = sensorId;
			currentParamName = paramName;
			currentParamFormula = paramFormula;

			chartTitleLabel->Text = "Dane pomiarowe: " + paramName + " (" + paramFormula + ")";
			measurementChart->ChartAreas[0]->AxisY->Title = paramFormula;

			chartPanel->Visible = true;
			chartPanel->BringToFront();

			RefreshChart();
		}

		// Funkcja pobieraj¹ca dane pomiarowe z API lub z pliku
		Json::Value GetMeasurementData(int sensorId) {
			Json::Value measurementData;

			try {
				measurementData = airQualityApi->getMeasurementData(sensorId);
			}
			catch (std::exception&) {
				statusLabel->Text = "Nie mo¿na po³¹czyæ siê z API, u¿ywam danych offline...";
			}

			// Jeœli nie uda³o siê pobraæ danych online, spróbuj z pliku
			if (measurementData.isNull() || measurementData["values"].isNull() || measurementData["values"].size() == 0) {
				Json::Value allMeasurements = airQualityApi->readJsonFromFile("all_measurements.json");
				std::string sensorIdStr = std::to_string(sensorId);

				if (allMeasurements.isMember(sensorIdStr)) {
					measurementData = allMeasurements[sensorIdStr];
				}
			}

			return measurementData;
		}
		
		// Funkcja analizuj¹ca trend danych
		String^ AnalyzeTrend(const std::vector<double>& values) {
			int size = values.size();
			if (size <= 5) {
				return "Brak wyraŸnego trendu (za ma³o danych)";
			}

			double sumFirst = 0;
			double sumLast = 0;

			for (int i = 0; i < size / 2; ++i) {
				sumFirst += values[i];
			}

			// Liczymy œredni¹ z drugiej po³owy
			for (int i = size / 2; i < size; ++i) {
				sumLast += values[i];
			}
			
			double avgFirst = sumFirst / (size / 2);
			double avgLast = sumLast / (size - size / 2); 



			// Okreœl trend 
			if (avgLast > avgFirst) {
				return  "Rosn¹cy";
			}
			else if (avgLast < avgFirst) {
				return  "Malej¹cy";
			}
			else {
				return "Stabilny";
			}
		}

		// Funkcja analizuj¹ca dane i zwracaj¹ca wyniki
		void AnalyzeData(Json::Value& measurementData, double& minValue, double& maxValue,
			double& average, int& count, std::string& minDate, std::string& maxDate,
			std::vector<double>& values) {
			minValue = 999999.0;
			maxValue = -999999.0;
			double sum = 0;
			count = 0;
			minDate = "";
			maxDate = "";
			values.clear();

			if (!measurementData["values"].isNull()) {
				for (const auto& measurement : measurementData["values"]) {
					if (!measurement["date"].isNull() && !measurement["value"].isNull()) {
						double value = measurement["value"].asDouble();
						sum += value;
						count++;
						values.push_back(value);

						// aktualizacja min
						if (value < minValue) {
							minValue = value;
							minDate = measurement["date"].asString();
						}

						// aktualizacja max
						if (value > maxValue) {
							maxValue = value;
							maxDate = measurement["date"].asString();
						}
					}
				}
			}

			// Oblicz œredni¹ jeœli s¹ dane
			if (count > 0) {
				average = sum / count;
			}
			else {
				average = 0;
			}
		}

		// Funkcja do odœwie¿ania wykresu z uwzglêdnieniem zakresu dat
		void RefreshChart() {
			try {
				Json::Value measurementData = GetMeasurementData(currentSensorId);

				// Wyczyœæ istniej¹ce serie danych
				measurementChart->Series->Clear();

				// SprawdŸ czy nazwa parametru nie jest pusta
				String^ seriesName = !String::IsNullOrEmpty(currentParamName) ? currentParamName : "Dane pomiarowe";

				// Utwórz now¹ seriê danych
				Series^ series = gcnew Series(seriesName);
				series->ChartType = SeriesChartType::Line;
				series->XValueType = ChartValueType::DateTime;

				// Okreœl zakres dat dla wykresu
				DateTime minDate = DateTime::Now;

				// Wybierz zakres dat na podstawie wybranej opcji
				switch (dateRangeComboBox->SelectedIndex) {
				case 0: minDate = DateTime::Now.AddHours(-24); break;
				case 1: minDate = DateTime::Now.AddDays(-3); break;
				case 2: minDate = DateTime::Now.AddDays(-7); break;
				case 3: minDate = DateTime::MinValue; break;
				default: minDate = DateTime::Now.AddHours(-24); break;
				}

				// Dodanie punktów danych do serii
				if (!measurementData["values"].isNull()) {
					for (const auto& measurement : measurementData["values"]) {
						if (!measurement["date"].isNull()) {
							std::string dateStr = measurement["date"].asString();
							String^ dateString = gcnew String(dateStr.c_str());

							DateTime measurementDate;
							if (DateTime::TryParse(dateString, measurementDate)) {
								if (measurementDate >= minDate) {
									if (!measurement["value"].isNull()) {
										double value = measurement["value"].asDouble();
										series->Points->AddXY(measurementDate, value);
									}
								}
							}
						}
					}
				}

				// Dodaj seriê do wykresu
				measurementChart->Series->Add(series);

				// Dostosuj zakres osi Y
				double minValue = 0;
				for (int i = 0; i < series->Points->Count; i++) {
					if (series->Points[i]->YValues[0] < minValue) {
						minValue = series->Points[i]->YValues[0];
					}
				}

				// Ustaw minimaln¹ wartoœæ osi Y
				if (minValue >= 0) {
					measurementChart->ChartAreas[0]->AxisY->Minimum = 0;
				}
				else {
					measurementChart->ChartAreas[0]->AxisY->Minimum = minValue * 1.1; // 10% poni¿ej minimum
				}

				// Aktualizuj informacje
				if (series->Points->Count == 0) {
					statusLabel->Text = "Brak danych pomiarowych w wybranym okresie";
				}
				else {
					statusLabel->Text = String::Format("Wyœwietlono {0} pomiarów", series->Points->Count);
					chartTitleLabel->Text = "Dane pomiarowe: " + currentParamName + " (" + currentParamFormula + ")";
					chartTitleLabel->ForeColor = System::Drawing::Color::White;
				}
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d pobierania danych pomiarowych: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
				statusLabel->Text = "B³¹d pobierania danych pomiarowych";
			}
		}

		// Analizuje i wyœwietla statystyki danych pomiarowych
		void AnalyzeMeasurementData() {
			try {
				Json::Value measurementData = GetMeasurementData(currentSensorId);

				double minValue, maxValue, average;
				int count;
				std::string minDate, maxDate;
				std::vector<double> values;

				AnalyzeData(measurementData, minValue, maxValue, average, count, minDate, maxDate, values);

				// Jeœli nie ma danych, wyœwietl komunikat
				if (count == 0) {
					MessageBox::Show(
						"Brak danych do analizy",
						"Informacja",
						MessageBoxButtons::OK,
						MessageBoxIcon::Information
					);
					return;
				}

				// Okreœlenie trendu 
				String^ trend = AnalyzeTrend(values);

				// Wyœwietl wyniki
				String^ analysisText = String::Format(
					"Analiza danych dla {0} ({1}):\n\n"
					"Wartoœæ minimalna: {2:F2} (data: {3})\n"
					"Wartoœæ maksymalna: {4:F2} (data: {5})\n"
					"Wartoœæ œrednia: {6:F2}\n"
					"Liczba pomiarów: {7}\n"
					"Trend: {8}",
					currentParamName,
					currentParamFormula,
					minValue,
					gcnew String(minDate.c_str()),
					maxValue,
					gcnew String(maxDate.c_str()),
					average,
					count,
					trend
				);
								MessageBox::Show(
					analysisText,
					"Analiza danych pomiarowych",
					MessageBoxButtons::OK,
					MessageBoxIcon::Information
				);
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d analizy danych: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
			}
		}

		// Konwertuje String^ na std::string
		std::string ConvertToStdString(String^ managedString) {
			if (managedString == nullptr)
				return "";
				
			msclr::interop::marshal_context context;
			return context.marshal_as<std::string>(managedString);
		}

		// Obs³uga przycisku "Zapisz"
		void saveButton_Click(System::Object^ sender, System::EventArgs^ e) {
			try {
				Json::Value measurementData = GetMeasurementData(currentSensorId);

				double minValue, maxValue, average;
				int count;
				std::string minDate, maxDate;
				std::vector<double> values;

				AnalyzeData(measurementData, minValue, maxValue, average, count, minDate, maxDate, values);

				// Jeœli nie ma danych, wyœwietl komunikat
				if (count == 0) {
					MessageBox::Show(
						"Brak danych do zapisania",
						"Informacja",
						MessageBoxButtons::OK,
						MessageBoxIcon::Information
					);
					return;
				}

				// Okreœlenie trendu
				String^ trend = AnalyzeTrend(values);
				std::string trendStr = ConvertToStdString(trend);

				// Utwórz obiekt JSON z wynikami analizy
				Json::Value analysisResult;

				// Konwersja z String^ na std::string
				std::string paramName = ConvertToStdString(currentParamName);
				std::string paramFormula = ConvertToStdString(currentParamFormula);

				// Informacje o parametrze
				analysisResult["parameter"]["name"] = paramName;
				analysisResult["parameter"]["formula"] = paramFormula;
				analysisResult["parameter"]["sensorId"] = currentSensorId;

				// Wyniki analizy
				analysisResult["analysis"]["minValue"] = minValue;
				analysisResult["analysis"]["minDate"] = minDate;
				analysisResult["analysis"]["maxValue"] = maxValue;
				analysisResult["analysis"]["maxDate"] = maxDate;
				analysisResult["analysis"]["average"] = average;
				analysisResult["analysis"]["count"] = count;
				analysisResult["analysis"]["trend"] = trendStr;

				// Zapisz wyniki do pliku
				std::string filename = "analiza_" + std::to_string(currentSensorId) + "_" + paramName + ".json";
				airQualityApi->saveJsonToFile(analysisResult, filename);

				MessageBox::Show(
					"Analiza zosta³a zapisana do pliku: " + gcnew String(filename.c_str()),
					"Informacja",
					MessageBoxButtons::OK,
					MessageBoxIcon::Information
				);
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d zapisywania analizy: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
			}
		}

		// Przycisk do pokazania wykresu
		void showChartButton_Click(System::Object^ sender, System::EventArgs^ e) {
			if (sensorsListView->SelectedItems->Count > 0) {
				ListViewItem^ selectedItem = sensorsListView->SelectedItems[0];
				int sensorId = safe_cast<int>(selectedItem->Tag);
				String^ paramName = selectedItem->SubItems[0]->Text;
				String^ paramFormula = selectedItem->SubItems[1]->Text;

				ShowMeasurementChart(sensorId, paramName, paramFormula);
			}
		}

		// Przycisk zamkniêcia wykresu
		void closeChartButton_Click(System::Object^ sender, System::EventArgs^ e) {
			chartPanel->Visible = false;
		}

		// Zmiana zakresu dat
		void dateRangeComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
			RefreshChart();
		}

		// Przycisk analizy danych
		void analyzeDataButton_Click(System::Object^ sender, System::EventArgs^ e) {
			AnalyzeMeasurementData();
		}

		// Wczytanie stacji z API lub pliku
		void LoadStations() {
			try {
				// Wyczyœæ listê
				stationsListView->Items->Clear();

				// Pobierz stacje z API
				Json::Value stations = airQualityApi->getAllStations();
				if (stations.size() == 0) {
					throw std::runtime_error("No stations data received from API");
				}
				
				// Zapisz dane do pliku dla u¿ytku offline
				airQualityApi->saveJsonToFile(stations, "stations.json");

				// Wyœwietl dane
				DisplayStationsData(stations);
			}
			catch (std::exception& ex) {
				// Poka¿ komunikat o b³êdzie
				MessageBox::Show(
					"Nie uda³o siê pobraæ informacji ze strony\nWyœwietlone zostan¹ dane archiwalne",
					"Komunikat",
					MessageBoxButtons::OK,
					MessageBoxIcon::Information
				);

				// Próba za³adowania danych z pliku lokalnego
				try {
					Json::Value cachedStations = airQualityApi->readJsonFromFile("stations.json");

					if (cachedStations.size() > 0) {
						DisplayStationsData(cachedStations);
						statusLabel->Text = String::Format("Za³adowano {0} stacji z lokalnej bazy", cachedStations.size());
					}
					else {
						MessageBox::Show(
							"Brak dostêpnych danych w pamiêci podrêcznej. Po³¹cz siê z internetem.",
							"B³¹d",
							MessageBoxButtons::OK,
							MessageBoxIcon::Error
						);
						statusLabel->Text = "Brak dostêpnych danych stacji";
					}
				}
				catch (std::exception& cacheEx) {
					MessageBox::Show(
						"B³¹d ³adowania danych z pamiêci podrêcznej: " + gcnew String(cacheEx.what()),
						"B³¹d",
						MessageBoxButtons::OK,
						MessageBoxIcon::Error
					);
					statusLabel->Text = "B³¹d ³adowania stacji";
				}
			}
		}

		// Wyœwietlanie listy stacji
		void DisplayStationsData(const Json::Value& stations) {
			if (stationsListView->Columns->Count == 0) {
				stationsListView->Columns->Add("Nazwa stacji", 350);
				stationsListView->Columns->Add("Miasto", 200);
			}
			
			for (const auto& station : stations) {
				std::string stationNameStr = station["stationName"].asString();
				String^ stationName = gcnew String(
					stationNameStr.c_str(),
					0,
					stationNameStr.length(),
					System::Text::Encoding::UTF8
				);
				
				String^ cityName = "";
				if (!station["city"].isNull() && !station["city"]["name"].isNull()) {
					std::string cityNameStr = station["city"]["name"].asString();
					cityName = gcnew String(
						cityNameStr.c_str(),
						0,
						cityNameStr.length(),
						System::Text::Encoding::UTF8
					);
				}
				
				ListViewItem^ item = gcnew ListViewItem(stationName);
				item->SubItems->Add(cityName);
				
				if (!station["id"].isNull()) {
					item->Tag = static_cast<int>(station["id"].asInt());
				}
				
				stationsListView->Items->Add(item);
			}
			
			statusLabel->Text = String::Format("Za³adowano {0} stacji pomiarowych", stations.size());
		}

		// Zarz¹dzanie polem wyszukiwania
		void searchBox_Enter(System::Object^ sender, System::EventArgs^ e) {
			if (searchBox->Text == "Search station") {
				searchBox->Text = "";
				searchBox->ForeColor = System::Drawing::Color::Black;
				searchBox->Font = gcnew System::Drawing::Font("Segoe UI", 10, System::Drawing::FontStyle::Regular);
			}
		}

		void searchBox_Leave(System::Object^ sender, System::EventArgs^ e) {
			if (String::IsNullOrWhiteSpace(searchBox->Text)) {
				searchBox->Text = "Search station";
				searchBox->ForeColor = System::Drawing::Color::Gray;
				searchBox->Font = gcnew System::Drawing::Font("Segoe UI", 10, System::Drawing::FontStyle::Italic);
			}
		}

		// Odœwie¿enie listy stacji
		void refreshButton_Click(System::Object^ sender, System::EventArgs^ e) {
			LoadStations();
		}

		// Wyszukiwanie stacji
		void searchBox_TextChanged(System::Object^ sender, System::EventArgs^ e) {
			String^ searchText = searchBox->Text->ToLower();

			// Przywrócenie wszystkich elementów, jeœli tekst wyszukiwania jest pusty
			if (String::IsNullOrWhiteSpace(searchText) || searchText->ToLower() == "search station") {
				LoadStations();
				return;
			}

			try {
				// Pobierz stacje z API lub pliku
				Json::Value stations = airQualityApi->getAllStations();
				if (stations.size() == 0) {
					stations = airQualityApi->readJsonFromFile("stations.json");
				}

				// Wyczyœæ listê
				stationsListView->Items->Clear();

				// Filtruj stacje
				for (const auto& station : stations) {
					std::string stationNameStr = station["stationName"].asString();
					String^ stationName = gcnew String(
						stationNameStr.c_str(),
						0,
						stationNameStr.length(),
						System::Text::Encoding::UTF8
					);

					String^ cityName = "";
					if (!station["city"].isNull() && !station["city"]["name"].isNull()) {
						std::string cityNameStr = station["city"]["name"].asString();
						cityName = gcnew String(
							cityNameStr.c_str(),
							0,
							cityNameStr.length(),
							System::Text::Encoding::UTF8
						);
					}

					// SprawdŸ, czy nazwa stacji lub miasta zawiera tekst wyszukiwania
					if (stationName->ToLower()->Contains(searchText) || cityName->ToLower()->Contains(searchText)) {
						ListViewItem^ item = gcnew ListViewItem(stationName);
						item->SubItems->Add(cityName);

						if (!station["id"].isNull()) {
							item->Tag = static_cast<int>(station["id"].asInt());
						}

						stationsListView->Items->Add(item);
					}
				}

				// Aktualizuj status
				statusLabel->Text = String::Format("Znaleziono {0} stacji pasuj¹cych do \"{1}\"",
					stationsListView->Items->Count,
					searchBox->Text);
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d podczas wyszukiwania: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
				statusLabel->Text = "Nie uda³o siê pobraæ informacji ze strony, sugerujemy u¿ycie danych archiwalnych";
			}
		}

		// Obs³uga wyboru stacji
		void stationsListView_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
			if (stationsListView->SelectedItems->Count > 0) {
				ListViewItem^ selectedItem = stationsListView->SelectedItems[0];
				int stationId = safe_cast<int>(selectedItem->Tag);
				String^ stationName = selectedItem->Text;

				sensorsTitleLabel->Text = "Stanowiska pomiarowe: " + stationName;

				LoadSensorsForStation(stationId);
				detailsPanel->Visible = true;
				stationsListView->Visible = false;
			}
		}

		// Obs³uga wyboru czujnika
		void sensorsListView_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
			showChartButton->Enabled = sensorsListView->SelectedItems->Count > 0;
		}

		// Przycisk powrotu
		void backButton_Click(System::Object^ sender, System::EventArgs^ e) {
			detailsPanel->Visible = false;
			stationsListView->Visible = true;
		}

		// £adowanie czujników dla wybranej stacji
		void LoadSensorsForStation(int stationId) {
			try {
				sensorsListView->Items->Clear();

				if (sensorsListView->Columns->Count == 0) {
					sensorsListView->Columns->Add("Nazwa parametru", 350);
					sensorsListView->Columns->Add("Symbol", 150);
				}

				// Pobierz czujniki dla wybranej stacji
				Json::Value sensors = airQualityApi->getSensorsByStation(stationId);

				// Jeœli nie ma danych z API, pobierz z pliku
				if (sensors.size() == 0) {
					Json::Value allSensors = airQualityApi->readJsonFromFile("all_sensors.json");

					// Filtruj czujniki dla wybranej stacji
					Json::Value filteredSensors(Json::arrayValue);
					for (const auto& sensor : allSensors) {
						if (!sensor["stationId"].isNull() && sensor["stationId"].asInt() == stationId) {
							filteredSensors.append(sensor);
						}
					}

					sensors = filteredSensors;

					if (sensors.size() == 0) {
						statusLabel->Text = "Nie znaleziono stanowisk pomiarowych dla tej stacji";
						return;
					}
					else {
						statusLabel->Text = "Za³adowano czujniki z bazy wszystkich czujników";
					}
				}
				else {
					statusLabel->Text = "Za³adowano dane czujników z API";
				}

				// Wyœwietl czujniki
				AddSensorsToListView(sensors);
			}
			catch (std::exception& ex) {
				MessageBox::Show(
					"B³¹d ³adowania czujników: " + gcnew String(ex.what()),
					"B³¹d",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
				statusLabel->Text = "B³¹d ³adowania czujników";

				TryLoadSensorsFromCache(stationId);
			}
		}

		// Próba wczytania czujników z cache
		void TryLoadSensorsFromCache(int stationId) {
			try {
				// Najpierw spróbuj z dedykowanego pliku dla tej stacji
				std::string cacheFileName = "sensors_" + std::to_string(stationId) + ".json";
				Json::Value cachedSensors = airQualityApi->readJsonFromFile(cacheFileName);

				// Jeœli nie uda³o siê za³adowaæ z pliku stacji, spróbuj z pliku all_sensors.json
				if (cachedSensors.size() == 0) {
					Json::Value allSensors = airQualityApi->readJsonFromFile("all_sensors.json");

					// Filtruj sensory dla wybranej stacji
					Json::Value filteredSensors(Json::arrayValue);
					for (const auto& sensor : allSensors) {
						if (!sensor["stationId"].isNull() && sensor["stationId"].asInt() == stationId) {
							filteredSensors.append(sensor);
						}
					}

					cachedSensors = filteredSensors;
				}

				if (cachedSensors.size() > 0) {
					AddSensorsToListView(cachedSensors);
					statusLabel->Text = String::Format("Za³adowano {0} czujników z pamiêci podrêcznej", cachedSensors.size());
				}
				else {
					statusLabel->Text = "Brak dostêpu do danych czujników";
				}
			}
			catch (std::exception&) {
				statusLabel->Text = "Brak dostêpu do danych czujników";
			}
		}

		// Dodanie czujników do listy
		void AddSensorsToListView(const Json::Value& sensors) {
			for (const auto& sensor : sensors) {
				String^ paramName = "";
				String^ paramFormula = "";

				if (!sensor["param"].isNull()) {
					if (!sensor["param"]["paramName"].isNull()) {
						std::string paramNameStr = sensor["param"]["paramName"].asString();
						paramName = gcnew String(
							paramNameStr.c_str(),
							0,
							paramNameStr.length(),
							System::Text::Encoding::UTF8
						);
					}

					if (!sensor["param"]["paramFormula"].isNull()) {
						std::string paramFormulaStr = sensor["param"]["paramFormula"].asString();
						paramFormula = gcnew String(
							paramFormulaStr.c_str(),
							0,
							paramFormulaStr.length(),
							System::Text::Encoding::UTF8
						);
					}
				}

				ListViewItem^ item = gcnew ListViewItem(paramName);
				item->SubItems->Add(paramFormula);

				if (!sensor["id"].isNull()) {
					item->Tag = static_cast<int>(sensor["id"].asInt());
				}

				sensorsListView->Items->Add(item);
			}

			statusLabel->Text = String::Format("Za³adowano {0} stanowisk pomiarowych", sensors.size());
		}

		// Zamkniêcie aplikacji
		void closeAppButton_Click(System::Object^ sender, System::EventArgs^ e) {
			this->Close();
		}
	};
}