#include "WFileWidget.h"

WFileWidget::WFileWidget(dyno::FBase* field)
{
	layout = this->setLayout(std::make_unique<Wt::WHBoxLayout>());
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	setValue(field);
	mfield = field;
	mfilename->textInput().connect(this, &WFileWidget::updateField);
}

WFileWidget::~WFileWidget() {}

std::string shortFilePath(std::string str)
{
	//std::size_t pos = str.find("data/");

	//if (pos != std::string::npos) {
	//	// "data/"֮��Ĳ���
	//	std::string dataPart = str.substr(pos + 5);
	//	std::cout << "��ȡ�����ݲ���: " << dataPart << std::endl;
	//	return dataPart;
	//}
	//else {
	//	std::cout << "δ�ҵ�'data/'����" << std::endl;
	//	return "Error";
	//}
	return "Error";
}

void WFileWidget::setValue(dyno::FBase* field)
{
	dyno::FVar<dyno::FilePath>* f = TypeInfo::cast<dyno::FVar<dyno::FilePath>>(field);
	if (f == nullptr)
		return;

	mfilename = layout->addWidget(std::make_unique<Wt::WLineEdit>());
	//std::string filepath = shortFilePath(f->getValue().string());
	mfilename->setText(f->getValue().string());
}

void WFileWidget::updateField()
{
	auto f = TypeInfo::cast<dyno::FVar<dyno::FilePath>>(mfield);
	if (f == nullptr)
	{
		return;
	}
	auto path = f->getValue();
	path.set_path(mfilename->text().toUTF8());
	f->setValue(path);
	f->update();
}