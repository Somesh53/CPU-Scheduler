// main.cpp
#include <wx/wx.h>
#include <vector>
#include <algorithm>
#include <queue>

struct Process {
    int id;
    wxString name;
    int arrivalTime;
    int burstTime;
    int priority;
    int startTime;
    int endTime;
    int remainingTime; // For Round Robin
};

class GanttChartPanel : public wxPanel {
public:
    GanttChartPanel(wxWindow* parent) : wxPanel(parent) {
        Bind(wxEVT_PAINT, &GanttChartPanel::OnPaint, this);
    }

    void SetProcesses(const std::vector<Process>& procs) {
        processes = procs;
        Refresh(); // Trigger a repaint
    }

private:
    std::vector<Process> processes;

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        DrawGanttChart(dc);
    }

    void DrawGanttChart(wxDC& dc);
};

void GanttChartPanel::DrawGanttChart(wxDC& dc) {
    if (processes.empty()) return;

    // Set background
    wxBrush backgroundBrush(*wxWHITE_BRUSH);
    dc.SetBackground(backgroundBrush);
    dc.Clear();

    // Get panel dimensions
    int panelWidth = GetSize().GetWidth();
    int panelHeight = GetSize().GetHeight();

    // Calculate the width for each time unit
    int totalTime = processes.back().endTime;
    int timeUnitWidth = panelWidth / (totalTime + 2);
    int boxHeight = 50;
    int padding = 30;

    std::vector<wxColour> colors = {
        wxColour(135, 206, 250),  // Light Sky Blue
        wxColour(173, 255, 47),   // Green Yellow
        wxColour(255, 182, 193),  // Light Pink
        wxColour(255, 215, 0),    // Gold
        wxColour(244, 164, 96),   // Sandy Brown
        wxColour(144, 238, 144),  // Light Green
        wxColour(221, 160, 221)   // Plum
    };

    // Draw the Gantt chart blocks
    int y = 50;
    for (const auto& process : processes) {
        int processWidth = (process.endTime - process.startTime) * timeUnitWidth;
        int colorIndex = (process.id - 1) % colors.size(); // Cycle through colors

        // Draw the process block
        wxBrush processBrush(colors[colorIndex]);
        dc.SetBrush(processBrush);
        dc.DrawRectangle(padding + process.startTime * timeUnitWidth, y, processWidth, boxHeight);

        wxString processName = process.name;
        int textWidth, textHeight;
        dc.GetTextExtent(processName, &textWidth, &textHeight);
        dc.DrawText(processName,
                    padding + process.startTime * timeUnitWidth + (processWidth - textWidth) / 2,
                    y + (boxHeight - textHeight) / 2);

        y += boxHeight + 20;
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxPen(wxColour(0, 0, 0)));
    y = panelHeight - padding;
    int timeY = y - 30;

    for (int time = 0; time <= totalTime; ++time) {
        int x = padding + time * timeUnitWidth;
        dc.DrawLine(x, timeY, x, y);
        wxString timeLabel = wxString::Format("%d", time);
        int textWidth, textHeight;
        dc.GetTextExtent(timeLabel, &textWidth, &textHeight);
        dc.DrawText(timeLabel, x - textWidth / 2, y + 5);
    }
}

class FCFSApp : public wxApp {
public:
    virtual bool OnInit();
};

class FCFSFrame : public wxFrame {
public:
    FCFSFrame(const wxString& title);

private:
    void OnAddProcess(wxCommandEvent& event);
    void OnSimulate(wxCommandEvent& event);

    wxTextCtrl* arrivalTimeCtrl;
    wxTextCtrl* burstTimeCtrl;
    wxTextCtrl* priorityCtrl;
    wxTextCtrl* nameCtrl; // Added name control
    wxChoice* algorithmChoice;
    GanttChartPanel* drawPanel;
    std::vector<Process> processes;
    wxDECLARE_EVENT_TABLE();

    void SimulateFCFS();
    void SimulateRoundRobin(int quantum);
    void SimulatePriorityScheduling();
    void SimulateSJF();
};

enum {
    ID_AddProcess = 1,
    ID_Simulate
};

wxBEGIN_EVENT_TABLE(FCFSFrame, wxFrame)
    EVT_BUTTON(ID_AddProcess, FCFSFrame::OnAddProcess)
    EVT_BUTTON(ID_Simulate, FCFSFrame::OnSimulate)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(FCFSApp);

bool FCFSApp::OnInit() {
    FCFSFrame* frame = new FCFSFrame("Scheduling Algorithms Simulator");
    frame->Show(true);
    return true;
}

FCFSFrame::FCFSFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxPanel* panel = new wxPanel(this, -1);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* arrivalTimeLabel = new wxStaticText(panel, wxID_ANY, wxT("Arrival Time: "));
    arrivalTimeCtrl = new wxTextCtrl(panel, wxID_ANY);
    wxStaticText* burstTimeLabel = new wxStaticText(panel, wxID_ANY, wxT("Burst Time: "));
    burstTimeCtrl = new wxTextCtrl(panel, wxID_ANY);
    wxStaticText* priorityLabel = new wxStaticText(panel, wxID_ANY, wxT("Priority: "));
    priorityCtrl = new wxTextCtrl(panel, wxID_ANY);
    wxStaticText* nameLabel = new wxStaticText(panel, wxID_ANY, wxT("Name: ")); // Added name label
    nameCtrl = new wxTextCtrl(panel, wxID_ANY); // Added name control
    wxButton* addButton = new wxButton(panel, ID_AddProcess, wxT("Add Process"));

    hbox1->Add(arrivalTimeLabel, 0, wxALL, 5);
    hbox1->Add(arrivalTimeCtrl, 0, wxALL, 5);
    hbox1->Add(burstTimeLabel, 0, wxALL, 5);
    hbox1->Add(burstTimeCtrl, 0, wxALL, 5);
    hbox1->Add(priorityLabel, 0, wxALL, 5);
    hbox1->Add(priorityCtrl, 0, wxALL, 5);
    hbox1->Add(nameLabel, 0, wxALL, 5); // Added name label
    hbox1->Add(nameCtrl, 0, wxALL, 5); // Added name control
    hbox1->Add(addButton, 0, wxALL, 5);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    wxArrayString algorithms;
    algorithms.Add("FCFS");
    algorithms.Add("Round Robin");
    algorithms.Add("Priority Scheduling");
    algorithms.Add("SJF");
    algorithmChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, algorithms);
    algorithmChoice->SetSelection(0);
    wxButton* simulateButton = new wxButton(panel, ID_Simulate, wxT("Simulate"));
    hbox2->Add(new wxStaticText(panel, wxID_ANY, wxT("Select Algorithm: ")), 0, wxALL, 5);
    hbox2->Add(algorithmChoice, 0, wxALL, 5);
    hbox2->Add(simulateButton, 0, wxALL, 5);

    drawPanel = new GanttChartPanel(panel);

    vbox->Add(hbox1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
    vbox->Add(hbox2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
    vbox->Add(drawPanel, 1, wxEXPAND | wxALL, 10);

    panel->SetSizer(vbox);
}

void FCFSFrame::OnAddProcess(wxCommandEvent& event) {
    long arrivalTime, burstTime, priority;
    wxString name = nameCtrl->GetValue(); // Get process name
    if (arrivalTimeCtrl->GetValue().ToLong(&arrivalTime) &&
        burstTimeCtrl->GetValue().ToLong(&burstTime) &&
        priorityCtrl->GetValue().ToLong(&priority) &&
        !name.IsEmpty()) { // Check if name is not empty
        Process newProcess = {
            static_cast<int>(processes.size() + 1),
            name, // Set process name
            static_cast<int>(arrivalTime),
            static_cast<int>(burstTime),
            static_cast<int>(priority),
            0, 0,
            static_cast<int>(burstTime) // Set remaining time for Round Robin
        };
        processes.push_back(newProcess);
        arrivalTimeCtrl->Clear();
        burstTimeCtrl->Clear();
        priorityCtrl->Clear();
        nameCtrl->Clear(); // Clear name control
    }
}

void FCFSFrame::OnSimulate(wxCommandEvent& event) {
    if (processes.empty()) return;

    int selection = algorithmChoice->GetSelection();
    switch (selection) {
        case 0: // FCFS
            SimulateFCFS();
            break;
        case 1: // Round Robin
            SimulateRoundRobin(2); // Quantum time is set to 2 for example
            break;
        case 2: // Priority Scheduling
            SimulatePriorityScheduling();
            break;
        case 3: // SJF
            SimulateSJF();
            break;
        default:
            break;
    }
}

void FCFSFrame::SimulateFCFS() {
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    for (auto& process : processes) {
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }
        process.startTime = currentTime;
        process.endTime = process.startTime + process.burstTime;
        currentTime = process.endTime;
    }

    drawPanel->SetProcesses(processes);
}

void FCFSFrame::SimulateRoundRobin(int quantum) {
    std::vector<Process> readyQueue;
    std::queue<Process*> processQueue;
    for (auto& process : processes) {
        processQueue.push(&process);
    }

    int currentTime = 0;
    while (!processQueue.empty()) {
        Process* process = processQueue.front();
        processQueue.pop();

        if (currentTime < process->arrivalTime) {
            currentTime = process->arrivalTime;
        }

        if (process->remainingTime > quantum) {
            process->startTime = currentTime;
            process->remainingTime -= quantum;
            currentTime += quantum;
            processQueue.push(process);
        } else {
            process->startTime = currentTime;
            currentTime += process->remainingTime;
            process->endTime = currentTime;
            process->remainingTime = 0;
            readyQueue.push_back(*process);
        }
    }

    drawPanel->SetProcesses(readyQueue);
}

void FCFSFrame::SimulatePriorityScheduling() {
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime || (a.arrivalTime == b.arrivalTime && a.priority > b.priority);
    });

    int currentTime = 0;
    for (auto& process : processes) {
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }
        process.startTime = currentTime;
        process.endTime = process.startTime + process.burstTime;
        currentTime = process.endTime;
    }

    drawPanel->SetProcesses(processes);
}

void FCFSFrame::SimulateSJF() {
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime || (a.arrivalTime == b.arrivalTime && a.burstTime < b.burstTime);
    });

    int currentTime = 0;
    for (auto& process : processes) {
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }
        process.startTime = currentTime;
        process.endTime = process.startTime + process.burstTime;
        currentTime = process.endTime;
    }

    drawPanel->SetProcesses(processes);
}
