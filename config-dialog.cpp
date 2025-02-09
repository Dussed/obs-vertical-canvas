#include "config-dialog.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QRadioButton>
#include <QPlainTextEdit>
#include <QCompleter>

#include "hotkey-edit.hpp"
#include "obs-module.h"
#include "version.h"
#include "vertical-canvas.hpp"
#include <util/dstr.h>

OBSBasicSettings::OBSBasicSettings(CanvasDock *canvas_dock, QMainWindow *parent)
	: QDialog(parent), canvasDock(canvas_dock)
{

	listWidget = new QListWidget(this);
	listWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	listWidget->setMaximumWidth(120);
	QListWidgetItem *listwidgetitem = new QListWidgetItem(listWidget);
	listwidgetitem->setIcon(QIcon(
		QString::fromUtf8(":/settings/images/settings/general.svg")));
	listwidgetitem->setText(QString::fromUtf8(obs_module_text("General")));

	listwidgetitem = new QListWidgetItem(listWidget);
	listwidgetitem->setIcon(QIcon(
		QString::fromUtf8(":/settings/images/settings/stream.svg")));
	listwidgetitem->setText(
		QString::fromUtf8(obs_module_text("Streaming")));

	listwidgetitem = new QListWidgetItem(listWidget);
	listwidgetitem->setIcon(QIcon(
		QString::fromUtf8(":/settings/images/settings/output.svg")));
	listwidgetitem->setText(
		QString::fromUtf8(obs_module_text("Recording")));

	listwidgetitem = new QListWidgetItem(listWidget);
	listwidgetitem->setIcon(
		canvasDock->GetIconFromType(OBS_ICON_TYPE_UNKNOWN));
	listwidgetitem->setText(QString::fromUtf8(obs_module_text("Help")));

	listWidget->setCurrentRow(0);

	auto settingsPages = new QStackedWidget;
	settingsPages->setContentsMargins(0, 0, 0, 0);
	settingsPages->setFrameShape(QFrame::NoFrame);
	settingsPages->setLineWidth(0);

	QWidget *generalPage = new QWidget;
	QScrollArea *scrollArea = new QScrollArea;
	scrollArea->setWidget(generalPage);
	scrollArea->setWidgetResizable(true);
	scrollArea->setLineWidth(0);
	scrollArea->setFrameShape(QFrame::NoFrame);
	settingsPages->addWidget(scrollArea);

	auto streamingPage = new QWidget;
	scrollArea = new QScrollArea;
	scrollArea->setWidget(streamingPage);
	scrollArea->setWidgetResizable(true);
	scrollArea->setLineWidth(0);
	scrollArea->setFrameShape(QFrame::NoFrame);
	settingsPages->addWidget(scrollArea);

	auto recordingPage = new QWidget;
	scrollArea = new QScrollArea;
	scrollArea->setWidget(recordingPage);
	scrollArea->setWidgetResizable(true);
	scrollArea->setLineWidth(0);
	scrollArea->setFrameShape(QFrame::NoFrame);
	settingsPages->addWidget(scrollArea);

	auto helpPage = new QWidget;
	scrollArea = new QScrollArea;
	scrollArea->setWidget(helpPage);
	scrollArea->setWidgetResizable(true);
	scrollArea->setLineWidth(0);
	scrollArea->setFrameShape(QFrame::NoFrame);
	settingsPages->addWidget(scrollArea);

	connect(listWidget, &QListWidget::currentRowChanged, settingsPages,
		&QStackedWidget::setCurrentIndex);

	auto generalGroup =
		new QGroupBox(QString::fromUtf8(obs_module_text("General")));

	auto generalLayout = new QFormLayout;
	generalLayout->setContentsMargins(9, 2, 9, 9);
	generalLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	generalLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

	generalGroup->setLayout(generalLayout);

	resolution = new QComboBox;
	resolution->setEditable(true);
	resolution->addItem("720x1280");
	resolution->addItem("1080x1920");
	resolution->addItem("1080x1350");
	generalLayout->addRow(QString::fromUtf8(obs_module_text("Resolution")),
			      resolution);

	showScenes =
		new QCheckBox(QString::fromUtf8(obs_module_text("ShowScenes")));
	generalLayout->addWidget(showScenes);

	videoBitrate = new QSpinBox;
	videoBitrate->setSuffix(" Kbps");
	videoBitrate->setMinimum(200);
	videoBitrate->setMaximum(1000000);
	generalLayout->addRow(QString::fromUtf8(obs_frontend_get_locale_string(
				      "Basic.Settings.Output.VideoBitrate")),
			      videoBitrate);

	audioBitrate = new QComboBox;
	audioBitrate->addItem("64", QVariant(64));
	audioBitrate->addItem("96", QVariant(96));
	audioBitrate->addItem("128", QVariant(128));
	audioBitrate->addItem("160", QVariant(160));
	audioBitrate->addItem("192", QVariant(192));
	audioBitrate->addItem("224", QVariant(224));
	audioBitrate->addItem("256", QVariant(256));
	audioBitrate->addItem("288", QVariant(288));
	audioBitrate->addItem("320", QVariant(320));

	audioBitrate->setCurrentText("160");
	generalLayout->addRow(QString::fromUtf8(obs_frontend_get_locale_string(
				      "Basic.Settings.Output.AudioBitrate")),
			      audioBitrate);

	auto backtrackGroup =
		new QGroupBox(QString::fromUtf8(obs_module_text("Backtrack")));
	auto backtrackLayout = new QFormLayout;
	backtrackLayout->setContentsMargins(9, 2, 9, 9);
	backtrackLayout->setFieldGrowthPolicy(
		QFormLayout::AllNonFixedFieldsGrow);
	backtrackLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
	backtrackGroup->setLayout(backtrackLayout);

	backtrackAlwaysOn = new QCheckBox(
		QString::fromUtf8(obs_module_text("BacktrackAlwaysOn")));
	backtrackLayout->addWidget(backtrackAlwaysOn);

	backtrackClip = new QCheckBox(
		QString::fromUtf8(obs_module_text("BacktrackEnable")));
	backtrackLayout->addWidget(backtrackClip);

	backtrackDuration = new QSpinBox;
	backtrackDuration->setSuffix(" sec");
	backtrackDuration->setMinimum(5);
	backtrackDuration->setMaximum(21600);
	backtrackLayout->addRow(
		QString::fromUtf8(obs_module_text("BacktrackDuration")),
		backtrackDuration);

	QLayout *backtrackPathLayout = new QHBoxLayout;
	backtrackPath = new QLineEdit();
	backtrackPath->setReadOnly(true);

	auto button = new QPushButton(
		QString::fromUtf8(obs_frontend_get_locale_string("Browse")));
	button->setProperty("themeID", "settingsButtons");
	connect(button, &QPushButton::clicked, [this] {
		const QString dir = QFileDialog::getExistingDirectory(
			this,
			QString::fromUtf8(obs_module_text("BacktrackPath")),
			backtrackPath->text(),
			QFileDialog::ShowDirsOnly |
				QFileDialog::DontResolveSymlinks);
		if (dir.isEmpty())
			return;
		backtrackPath->setText(dir);
	});

	backtrackPathLayout->addWidget(backtrackPath);
	backtrackPathLayout->addWidget(button);

	backtrackLayout->addRow(
		QString::fromUtf8(obs_module_text("BacktrackPath")),
		backtrackPathLayout);

	auto replayHotkeys = GetHotKeysFromOutput(canvasDock->replayOutput);

	for (auto &hotkey : replayHotkeys) {
		auto id = obs_hotkey_get_id(hotkey);
		std::vector<obs_key_combination_t> combos =
			GetCombosForHotkey(id);
		auto hn = obs_hotkey_get_name(hotkey);
		auto hw = new OBSHotkeyWidget(this, id, hn, combos);
		if (strcmp(hn, "ReplayBuffer.Save") == 0) {
			backtrackLayout->addRow(
				QString::fromUtf8(
					obs_module_text("SaveBacktrackHotkey")),
				hw);
		} else {
			backtrackLayout->addRow(
				QString::fromUtf8(
					obs_hotkey_get_description(hotkey)),
				hw);
		}
		hotkeys.push_back(hw);
	}
	auto maxWidth = 180;
	for (int row = 0; row < generalLayout->rowCount(); row++) {
		auto item = generalLayout->itemAt(row, QFormLayout::LabelRole);
		if (!item)
			continue;
		auto label = dynamic_cast<QLabel *>(item->widget());
		if (!label)
			continue;
		label->setFixedWidth(maxWidth);
		label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	}
	for (int row = 0; row < backtrackLayout->rowCount(); row++) {
		auto item =
			backtrackLayout->itemAt(row, QFormLayout::LabelRole);
		if (!item)
			continue;
		auto label = dynamic_cast<QLabel *>(item->widget());
		if (!label)
			continue;
		label->setFixedWidth(maxWidth);
		label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	}

	auto vb = new QVBoxLayout;
	vb->setContentsMargins(0, 0, 0, 0);
	vb->addWidget(generalGroup);
	vb->addWidget(backtrackGroup);
	vb->addStretch();
	generalPage->setLayout(vb);

	auto streamingGroup =
		new QGroupBox(QString::fromUtf8(obs_module_text("Streaming")));
	streamingGroup->setSizePolicy(QSizePolicy::Preferred,
				      QSizePolicy::Maximum);

	auto streamingLayout = new QFormLayout;
	streamingLayout->setContentsMargins(9, 2, 9, 9);
	streamingLayout->setFieldGrowthPolicy(
		QFormLayout::AllNonFixedFieldsGrow);
	streamingLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignTrailing |
					   Qt::AlignVCenter);

	server = new QComboBox;
	server->setEditable(true);

	server->addItem("rtmps://a.rtmps.youtube.com:443/live2");
	server->addItem("rtmps://b.rtmps.youtube.com:443/live2?backup=1");
	server->addItem("rtmp://a.rtmp.youtube.com/live2");
	server->addItem("rtmp://b.rtmp.youtube.com/live2?backup=1");
	streamingLayout->addRow(QString::fromUtf8(obs_module_text("Server")),
				server);

	QLayout *subLayout = new QHBoxLayout();
	key = new QLineEdit;
	key->setEchoMode(QLineEdit::Password);

	QPushButton *show = new QPushButton();
	show->setText(
		QString::fromUtf8(obs_frontend_get_locale_string("Show")));
	show->setCheckable(true);
	connect(show, &QAbstractButton::toggled, [=](bool hide) {
		show->setText(QString::fromUtf8(
			hide ? obs_frontend_get_locale_string("Hide")
			     : obs_frontend_get_locale_string("Show")));
		key->setEchoMode(hide ? QLineEdit::Normal
				      : QLineEdit::Password);
	});

	subLayout->addWidget(key);
	subLayout->addWidget(show);

	streamingLayout->addRow(QString::fromUtf8(obs_module_text("Key")),
				subLayout);

	OBSHotkeyWidget *otherHotkey = nullptr;
	auto hotkey =
		GetHotkeyByName(canvasDock->objectName() + "StartStreaming");
	if (hotkey) {
		auto id = obs_hotkey_get_id(hotkey);
		std::vector<obs_key_combination_t> combos =
			GetCombosForHotkey(id);
		auto hn = obs_hotkey_get_name(hotkey);
		auto hw = new OBSHotkeyWidget(this, id, hn, combos);
		otherHotkey = hw;
		auto label = new OBSHotkeyLabel;
		label->setText(QString::fromUtf8(
			obs_module_text("StartStreamingHotkey")));
		hw->label = label;
		streamingLayout->addRow(label, hw);
		hotkeys.push_back(hw);
	}

	hotkey = GetHotkeyByName(canvasDock->objectName() + "StopStreaming");
	if (hotkey) {
		auto id = obs_hotkey_get_id(hotkey);
		std::vector<obs_key_combination_t> combos =
			GetCombosForHotkey(id);
		auto hn = obs_hotkey_get_name(hotkey);
		auto hw = new OBSHotkeyWidget(this, id, hn, combos);
		auto label = new OBSHotkeyLabel;
		label->setText(QString::fromUtf8(
			obs_module_text("StopStreamingHotkey")));
		hw->label = label;
		streamingLayout->addRow(label, hw);
		hotkeys.push_back(hw);
		if (otherHotkey) {
			hw->label->pairPartner = otherHotkey->label;
			otherHotkey->label->pairPartner = hw->label;
		}
	}

	streamingGroup->setLayout(streamingLayout);

	auto streamingAdvancedGroup = new QGroupBox(QString::fromUtf8(
		obs_frontend_get_locale_string("Basic.Settings.Advanced")));
	auto streamingAdvancedLayout = new QFormLayout;
	streamingAdvancedLayout->setContentsMargins(9, 2, 9, 9);
	streamingAdvancedLayout->setFieldGrowthPolicy(
		QFormLayout::AllNonFixedFieldsGrow);
	streamingAdvancedLayout->setLabelAlignment(
		Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

	streamingUseMain =
		new QCheckBox(QString::fromUtf8(obs_module_text("UseMain")));
	connect(streamingUseMain, &QCheckBox::stateChanged,
		[this, streamingAdvancedLayout] {
			bool checked = streamingUseMain->isChecked();
			for (int i = 1; i < streamingAdvancedLayout->rowCount();
			     i++) {
				auto field = streamingAdvancedLayout->itemAt(
					i, QFormLayout::FieldRole);
				if (field) {
					auto w = field->widget();
					if (w)
						w->setEnabled(!checked);
				}
				auto label = streamingAdvancedLayout->itemAt(
					i, QFormLayout::LabelRole);
				if (label) {
					auto w = label->widget();
					if (w)
						w->setEnabled(!checked);
				}
			}
		});
	streamingAdvancedLayout->addWidget(streamingUseMain);

	auto audioTracks = new QFrame;
	audioTracks->setSizePolicy(QSizePolicy::Maximum,
				   QSizePolicy::Preferred);
	audioTracks->setContentsMargins(0, 0, 0, 0);
	auto al = new QHBoxLayout;
	al->setContentsMargins(0, 0, 0, 0);
	audioTracks->setLayout(al);
	for (int i = 1; i <= MAX_AUDIO_MIXES; i++) {
		auto radio = new QRadioButton(QString::fromUtf8("%1").arg(i));
		streamingAudioTracks.push_back(radio);
		al->addWidget(radio);
	}
	streamingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			"Basic.Settings.Output.Adv.AudioTrack")),
		audioTracks);

	streamingEncoder = new QComboBox;
	const char *type;
	size_t idx = 0;
	while (obs_enum_encoder_types(idx++, &type)) {
		if (obs_get_encoder_type(type) != OBS_ENCODER_VIDEO)
			continue;
		uint32_t caps = obs_get_encoder_caps(type);
		if ((caps & (OBS_ENCODER_CAP_DEPRECATED |
			     OBS_ENCODER_CAP_INTERNAL)) != 0)
			continue;
		const char *codec = obs_get_encoder_codec(type);
		if (astrcmpi(codec, "h264") != 0 &&
		    astrcmpi(codec, "hevc") != 0)
			continue;
		streamingEncoder->addItem(
			QString::fromUtf8(obs_encoder_get_display_name(type)),
			QVariant(QString::fromUtf8(type)));
	}

	connect(streamingEncoder, &QComboBox::currentIndexChanged,
		[streamingAdvancedLayout, this] {
			int row = 0;
			streamingAdvancedLayout->getWidgetPosition(
				streamingEncoder, &row, nullptr);
			for (int i = streamingAdvancedLayout->rowCount() - 1;
			     i > row; i--) {
				streamingAdvancedLayout->removeRow(i);
			}
			if (streamingEncoder->currentIndex() < 0)
				return;
			auto encoder_string = streamingEncoder->currentData()
						      .toString()
						      .toUtf8();
			auto encoder = encoder_string.constData();
			obs_data_release(stream_encoder_settings);
			stream_encoder_settings = obs_encoder_defaults(encoder);
			obs_data_apply(stream_encoder_settings,
				       canvasDock->stream_encoder_settings);
			if (stream_encoder_properties)
				obs_properties_destroy(
					stream_encoder_properties);
			stream_encoder_property_widgets.clear();
			stream_encoder_properties =
				obs_get_encoder_properties(encoder);

			obs_property_t *property =
				obs_properties_first(stream_encoder_properties);
			while (property) {
				AddProperty(property, stream_encoder_settings,
					    streamingAdvancedLayout,
					    &stream_encoder_property_widgets);
				obs_property_next(&property);
			}
			QMetaObject::invokeMethod(
				streamingUseMain, "stateChanged",
				Q_ARG(int, streamingUseMain->checkState()));
		});
	streamingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			(obs_get_version() >= MAKE_SEMANTIC_VERSION(29, 1, 0) ||
			 strncmp(obs_get_version_string(), "29.1.", 5) == 0)
				? "Basic.Settings.Output.Encoder.Video"
				: "Basic.Settings.Output.Encoder")),
		streamingEncoder);
	QMetaObject::invokeMethod(streamingEncoder, "currentIndexChanged",
				  Q_ARG(int, streamingEncoder->currentIndex()));

	streamingAdvancedGroup->setLayout(streamingAdvancedLayout);

	vb = new QVBoxLayout;
	vb->setContentsMargins(0, 0, 0, 0);
	vb->addWidget(streamingGroup);
	vb->addWidget(streamingAdvancedGroup);
	vb->addStretch();
	streamingPage->setLayout(vb);

	auto recordGroup =
		new QGroupBox(QString::fromUtf8(obs_module_text("Recording")));
	auto recordLayout = new QFormLayout;
	recordLayout->setContentsMargins(9, 2, 9, 9);
	recordLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	recordLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

	QLayout *recordPathLayout = new QHBoxLayout();
	recordPath = new QLineEdit();
	recordPath->setReadOnly(true);

	button = new QPushButton(
		QString::fromUtf8(obs_frontend_get_locale_string("Browse")));
	button->setProperty("themeID", "settingsButtons");
	connect(button, &QPushButton::clicked, [this] {
		const QString dir = QFileDialog::getExistingDirectory(
			this,
			QString::fromUtf8(obs_frontend_get_locale_string(
				"Basic.Settings.Output.Simple.SavePath")),
			backtrackPath->text(),
			QFileDialog::ShowDirsOnly |
				QFileDialog::DontResolveSymlinks);
		if (dir.isEmpty())
			return;
		recordPath->setText(dir);
	});

	recordPathLayout->addWidget(recordPath);
	recordPathLayout->addWidget(button);

	recordLayout->addRow(QString::fromUtf8(obs_frontend_get_locale_string(
				     "Basic.Settings.Output.Simple.SavePath")),
			     recordPathLayout);

	otherHotkey = nullptr;

	hotkey = GetHotkeyByName(canvasDock->objectName() + "StartRecording");
	if (hotkey) {
		auto id = obs_hotkey_get_id(hotkey);
		std::vector<obs_key_combination_t> combos =
			GetCombosForHotkey(id);
		auto hn = obs_hotkey_get_name(hotkey);
		auto hw = new OBSHotkeyWidget(this, id, hn, combos);
		otherHotkey = hw;
		auto label = new OBSHotkeyLabel;
		label->setText(QString::fromUtf8(
			obs_module_text("StartRecordingHotkey")));
		hw->label = label;
		recordLayout->addRow(label, hw);
		hotkeys.push_back(hw);
	}

	hotkey = GetHotkeyByName(canvasDock->objectName() + "StopRecording");
	if (hotkey) {
		auto id = obs_hotkey_get_id(hotkey);
		std::vector<obs_key_combination_t> combos =
			GetCombosForHotkey(id);
		auto hn = obs_hotkey_get_name(hotkey);
		auto hw = new OBSHotkeyWidget(this, id, hn, combos);

		auto label = new OBSHotkeyLabel;
		label->setText(QString::fromUtf8(
			obs_module_text("StopRecordingHotkey")));
		hw->label = label;
		recordLayout->addRow(label, hw);
		hotkeys.push_back(hw);
		if (otherHotkey) {
			hw->label->pairPartner = otherHotkey->label;
			otherHotkey->label->pairPartner = hw->label;
		}
	}

	recordGroup->setLayout(recordLayout);

	auto recordingAdvancedGroup = new QGroupBox(QString::fromUtf8(
		obs_frontend_get_locale_string("Basic.Settings.Advanced")));
	auto recordingAdvancedLayout = new QFormLayout;
	recordingAdvancedLayout->setContentsMargins(9, 2, 9, 9);
	recordingAdvancedLayout->setFieldGrowthPolicy(
		QFormLayout::AllNonFixedFieldsGrow);
	recordingAdvancedLayout->setLabelAlignment(
		Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

	recordingUseMain =
		new QCheckBox(QString::fromUtf8(obs_module_text("UseMain")));
	connect(recordingUseMain, &QCheckBox::stateChanged,
		[this, recordingAdvancedLayout] {
			bool checked = recordingUseMain->isChecked();
			for (int i = 1; i < recordingAdvancedLayout->rowCount();
			     i++) {
				auto field = recordingAdvancedLayout->itemAt(
					i, QFormLayout::FieldRole);
				if (field) {
					auto w = field->widget();
					if (w)
						w->setEnabled(!checked);
				}
				auto label = recordingAdvancedLayout->itemAt(
					i, QFormLayout::LabelRole);
				if (label) {
					auto w = label->widget();
					if (w)
						w->setEnabled(!checked);
				}
			}
		});
	recordingAdvancedLayout->addWidget(recordingUseMain);

	filenameFormat = new QLineEdit;

	QStringList specList =
		QString::fromUtf8(obs_frontend_get_locale_string(
					  "FilenameFormatting.completer"))
			.split(QRegularExpression("\n"));
	filenameFormat->setText(specList.first());
	QCompleter *specCompleter = new QCompleter(specList);
	specCompleter->setCaseSensitivity(Qt::CaseSensitive);
	specCompleter->setFilterMode(Qt::MatchContains);
	filenameFormat->setCompleter(specCompleter);
	filenameFormat->setToolTip(QString::fromUtf8(
		obs_frontend_get_locale_string("FilenameFormatting.TT")));

	connect(filenameFormat, &QLineEdit::textEdited,
		[this](const QString &text) {
			QString safeStr = text;

#ifdef __APPLE__
			safeStr.replace(QRegularExpression("[:]"), "");
#elif defined(_WIN32)
	safeStr.replace(QRegularExpression("[<>:\"\\|\\?\\*]"), "");
#else
		// TODO: Add filtering for other platforms
#endif
			if (text != safeStr)
				filenameFormat->setText(safeStr);
		});

	recordingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			"Basic.Settings.Output.Adv.Recording.Filename")),
		filenameFormat);

	fileFormat = new QComboBox;
	fileFormat->addItem(QString::fromUtf8("mp4"));
	fileFormat->addItem(QString::fromUtf8("mov"));
	fileFormat->addItem(QString::fromUtf8("mkv"));
	fileFormat->addItem(QString::fromUtf8("ts"));
	fileFormat->addItem(QString::fromUtf8("mp3u8"));

	recordingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			"Basic.Settings.Output.Format")),
		fileFormat);

	audioTracks = new QFrame;
	audioTracks->setSizePolicy(QSizePolicy::Maximum,
				   QSizePolicy::Preferred);
	audioTracks->setContentsMargins(0, 0, 0, 0);
	al = new QHBoxLayout;
	al->setContentsMargins(0, 0, 0, 0);
	audioTracks->setLayout(al);
	for (int i = 1; i <= MAX_AUDIO_MIXES; i++) {
		auto check = new QCheckBox(QString::fromUtf8("%1").arg(i));
		recordingAudioTracks.push_back(check);
		al->addWidget(check);
	}
	recordingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			"Basic.Settings.Output.Adv.AudioTrack")),
		audioTracks);

	recordingEncoder = new QComboBox;
	recordingEncoder->addItem(
		QString::fromUtf8(obs_frontend_get_locale_string(
			"Basic.Settings.Output.Adv.Recording.UseStreamEncoder")),
		QVariant(QString::fromUtf8("")));
	idx = 0;
	while (obs_enum_encoder_types(idx++, &type)) {
		if (obs_get_encoder_type(type) != OBS_ENCODER_VIDEO)
			continue;
		uint32_t caps = obs_get_encoder_caps(type);
		if ((caps & (OBS_ENCODER_CAP_DEPRECATED |
			     OBS_ENCODER_CAP_INTERNAL)) != 0)
			continue;
		recordingEncoder->addItem(
			QString::fromUtf8(obs_encoder_get_display_name(type)),
			QVariant(QString::fromUtf8(type)));
	}

	connect(recordingEncoder, &QComboBox::currentIndexChanged,
		[recordingAdvancedLayout, this] {
			int row = 0;
			recordingAdvancedLayout->getWidgetPosition(
				recordingEncoder, &row, nullptr);
			for (int i = recordingAdvancedLayout->rowCount() - 1;
			     i > row; i--) {
				recordingAdvancedLayout->removeRow(i);
			}
			if (record_encoder_properties)
				obs_properties_destroy(
					record_encoder_properties);
			record_encoder_properties = nullptr;
			record_encoder_property_widgets.clear();
			if (recordingEncoder->currentIndex() < 1)
				return;
			auto encoder_string = recordingEncoder->currentData()
						      .toString()
						      .toUtf8();
			auto encoder = encoder_string.constData();
			obs_data_release(record_encoder_settings);
			record_encoder_settings = obs_encoder_defaults(encoder);
			obs_data_apply(record_encoder_settings,
				       canvasDock->record_encoder_settings);
			record_encoder_properties =
				obs_get_encoder_properties(encoder);

			obs_property_t *property =
				obs_properties_first(record_encoder_properties);
			while (property) {
				AddProperty(property, record_encoder_settings,
					    recordingAdvancedLayout,
					    &record_encoder_property_widgets);
				obs_property_next(&property);
			}
			QMetaObject::invokeMethod(
				recordingUseMain, "stateChanged",
				Q_ARG(int, recordingUseMain->checkState()));
		});

	recordingAdvancedLayout->addRow(
		QString::fromUtf8(obs_frontend_get_locale_string(
			(obs_get_version() >= MAKE_SEMANTIC_VERSION(29, 1, 0) ||
			 strncmp(obs_get_version_string(), "29.1.", 5) == 0)
				? "Basic.Settings.Output.Encoder.Video"
				: "Basic.Settings.Output.Encoder")),
		recordingEncoder);

	recordingAdvancedGroup->setLayout(recordingAdvancedLayout);

	vb = new QVBoxLayout;
	vb->setContentsMargins(0, 0, 0, 0);
	vb->addWidget(recordGroup);
	vb->addWidget(recordingAdvancedGroup);
	vb->addStretch();
	recordingPage->setLayout(vb);

	auto helpGroup =
		new QGroupBox(QString::fromUtf8(obs_module_text("Help")));

	auto helpLayout = new QVBoxLayout;

	auto helpLabel = new QLabel(
		QString::fromUtf8("<a href = 'https://aitum.tv/discord'>") +
		QString::fromUtf8(obs_module_text("HelpSupport")) +
		QString::fromUtf8("</a>"));
	helpLabel->setOpenExternalLinks(true);

	helpLayout->addWidget(helpLabel);

	helpGroup->setLayout(helpLayout);

	vb = new QVBoxLayout;
	vb->setContentsMargins(0, 0, 0, 0);
	vb->addWidget(helpGroup);
	vb->addStretch();
	helpPage->setLayout(vb);

	QPushButton *okButton = new QPushButton(
		QString::fromUtf8(obs_frontend_get_locale_string("OK")));
	connect(okButton, &QPushButton::clicked, [this] {
		SaveSettings();
		close();
	});

	QPushButton *cancelButton = new QPushButton(
		QString::fromUtf8(obs_frontend_get_locale_string("Cancel")));
	connect(cancelButton, &QPushButton::clicked, [this] { close(); });

	QHBoxLayout *contentLayout = new QHBoxLayout;
	contentLayout->addWidget(listWidget);

	contentLayout->addWidget(settingsPages, 1);

	QHBoxLayout *bottomLayout = new QHBoxLayout;
	const auto version =
		new QLabel(QString::fromUtf8(obs_module_text("Version")) + " " +
			   QString::fromUtf8(PROJECT_VERSION) + " " +
			   QString::fromUtf8(obs_module_text("MadeBy")) +
			   " <a href=\"https://aitum.tv\">Aitum</a>");
	version->setOpenExternalLinks(true);
	version->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	newVersion = new QLabel;
	newVersion->setProperty("themeID", "warning");
	newVersion->setVisible(false);
	newVersion->setOpenExternalLinks(true);
	newVersion->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	bottomLayout->addWidget(version, 1, Qt::AlignLeft);
	bottomLayout->addWidget(newVersion, 1, Qt::AlignLeft);
	bottomLayout->addWidget(okButton, 0, Qt::AlignRight);
	bottomLayout->addWidget(cancelButton, 0, Qt::AlignRight);

	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(11, 11, 11, 11);
	vlayout->addLayout(contentLayout);
	vlayout->addLayout(bottomLayout);
	setLayout(vlayout);

	setWindowTitle(obs_module_text("VerticalSettings"));
	setSizeGripEnabled(true);
}

OBSBasicSettings::~OBSBasicSettings()
{
	obs_properties_destroy(stream_encoder_properties);
	obs_properties_destroy(record_encoder_properties);
	obs_data_release(stream_encoder_settings);
	obs_data_release(record_encoder_settings);
}

QIcon OBSBasicSettings::GetGeneralIcon() const
{
	return generalIcon;
}

QIcon OBSBasicSettings::GetStreamIcon() const
{
	return streamIcon;
}

QIcon OBSBasicSettings::GetOutputIcon() const
{
	return outputIcon;
}

void OBSBasicSettings::SetGeneralIcon(const QIcon &icon)
{
	listWidget->item(0)->setIcon(icon);
}

void OBSBasicSettings::SetStreamIcon(const QIcon &icon)
{
	listWidget->item(1)->setIcon(icon);
}

void OBSBasicSettings::SetOutputIcon(const QIcon &icon)
{
	listWidget->item(2)->setIcon(icon);
}

void OBSBasicSettings::LoadSettings()
{
	if (!canvasDock->newer_version_available.isEmpty()) {
		newVersion->setText(
			QString::fromUtf8(obs_module_text("NewVersion"))
				.arg(canvasDock->newer_version_available));
		newVersion->setVisible(true);
	}
	resolution->setCurrentText(QString::number(canvasDock->canvas_width) +
				   "x" +
				   QString::number(canvasDock->canvas_height));
	resolution->setEnabled(
		!obs_output_active(canvasDock->recordOutput) &&
		!obs_output_active(canvasDock->streamOutput) &&
		!obs_output_active(canvasDock->virtualCamOutput));
	showScenes->setChecked(!canvasDock->hideScenes);
	videoBitrate->setValue(
		canvasDock->videoBitrate ? canvasDock->videoBitrate : 6000);
	for (int i = 0; i < audioBitrate->count(); i++) {
		if (audioBitrate->itemData(i).toUInt() ==
		    (canvasDock->audioBitrate ? canvasDock->audioBitrate
					      : 160)) {
			audioBitrate->setCurrentIndex(i);
		}
	}
	backtrackClip->setChecked(canvasDock->startReplay);
	backtrackAlwaysOn->setChecked(canvasDock->replayAlwaysOn);
	backtrackDuration->setValue(canvasDock->replayDuration);
	backtrackPath->setText(QString::fromUtf8(canvasDock->replayPath));

	key->setEchoMode(QLineEdit::Password);
	key->setText(QString::fromUtf8(canvasDock->stream_key));
	server->setCurrentText(QString::fromUtf8(canvasDock->stream_server));

	streamingUseMain->setChecked(!canvasDock->stream_advanced_settings);
	if (canvasDock->stream_audio_track > 0)
		streamingAudioTracks[canvasDock->stream_audio_track - 1]
			->setChecked(true);

	auto idx = streamingEncoder->findData(QVariant(
		QString::fromUtf8(canvasDock->stream_encoder.c_str())));
	if (idx != -1)
		streamingEncoder->setCurrentIndex(idx);

	for (const auto &kv : stream_encoder_property_widgets) {
		LoadProperty(kv.first, canvasDock->stream_encoder_settings,
			     kv.second);
	}

	recordPath->setText(QString::fromUtf8(canvasDock->recordPath));

	recordingUseMain->setChecked(!canvasDock->record_advanced_settings);
	if (!canvasDock->filename_formatting.empty())
		filenameFormat->setText(
			canvasDock->filename_formatting.c_str());
	idx = fileFormat->findData(
		QVariant(QString::fromUtf8(canvasDock->file_format.c_str())));
	if (idx == -1) {
		idx = fileFormat->findText(
			QString::fromUtf8(canvasDock->file_format.c_str()));
	}
	if (idx != 1)
		fileFormat->setCurrentIndex(idx);

	for (size_t i = 0; i < MAX_AUDIO_MIXES; i++) {
		recordingAudioTracks[i]->setChecked(
			(canvasDock->record_audio_tracks & (1ll << i)) != 0);
	}

	idx = recordingEncoder->findData(QVariant(
		QString::fromUtf8(canvasDock->record_encoder.c_str())));
	if (idx != -1)
		recordingEncoder->setCurrentIndex(idx);

	for (const auto &kv : record_encoder_property_widgets) {
		LoadProperty(kv.first, canvasDock->record_encoder_settings,
			     kv.second);
	}
}

void OBSBasicSettings::SaveSettings()
{
	for (auto &hw : hotkeys) {
		hw->Save();
	}

	if (canvasDock->hideScenes == showScenes->isChecked()) {
		canvasDock->hideScenes = !showScenes->isChecked();
		auto sl = canvasDock->GetGlobalScenesList();
		for (int j = 0; j < sl->count(); j++) {
			auto item = sl->item(j);
			if (canvasDock->HasScene(item->text())) {
				item->setHidden(canvasDock->hideScenes);
			}
		}
	}
	const auto res = resolution->currentText();
	uint32_t width, height;
	if (sscanf(res.toUtf8().constData(), "%dx%d", &width, &height) == 2 &&
	    width && height &&
	    (width != canvasDock->canvas_width ||
	     height != canvasDock->canvas_height)) {
		if (obs_output_active(canvasDock->replayOutput))
			obs_output_stop(canvasDock->replayOutput);

		canvasDock->canvas_width = width;
		canvasDock->canvas_height = height;

		canvasDock->ResizeScenes();
		auto t = obs_weak_source_get_source(canvasDock->source);
		if (obs_source_get_type(t) == OBS_SOURCE_TYPE_TRANSITION) {
			obs_transition_set_size(t, width, height);
		}
		obs_source_release(t);

		const QString name = "CanvasDock" + QString::number(width) +
				     "x" + QString::number(height);
		canvasDock->setObjectName(name);

		if (canvasDock->scenesDock)
			canvasDock->scenesDock->setObjectName(name + "Scenes");

		if (canvasDock->sourcesDock)
			canvasDock->sourcesDock->setObjectName(name +
							       "Sources");

		canvasDock->LoadScenes();
		canvasDock->ProfileChanged();
	}
	uint32_t bitrate = (uint32_t)videoBitrate->value();
	if (bitrate != canvasDock->videoBitrate) {
		canvasDock->videoBitrate = bitrate;
		SetEncoderBitrate(
			obs_output_get_video_encoder(canvasDock->replayOutput));
		SetEncoderBitrate(
			obs_output_get_video_encoder(canvasDock->recordOutput));
		SetEncoderBitrate(
			obs_output_get_video_encoder(canvasDock->streamOutput));
	}
	bitrate = (uint32_t)audioBitrate->currentData().toUInt();
	if (bitrate != canvasDock->audioBitrate) {
		canvasDock->audioBitrate = bitrate;
		for (size_t i = 0; i < MAX_AUDIO_MIXES; i++) {
			SetEncoderBitrate(obs_output_get_audio_encoder(
				canvasDock->replayOutput, i));
			SetEncoderBitrate(obs_output_get_audio_encoder(
				canvasDock->recordOutput, i));
			SetEncoderBitrate(obs_output_get_audio_encoder(
				canvasDock->streamOutput, i));
		}
	}

	auto startReplay = backtrackClip->isChecked();
	auto replayAlwaysOn = backtrackAlwaysOn->isChecked();
	auto duration = (uint32_t)backtrackDuration->value();
	std::string replayPath = backtrackPath->text().toUtf8().constData();
	if (duration != canvasDock->replayDuration ||
	    replayPath != canvasDock->replayPath ||
	    canvasDock->startReplay != startReplay ||
	    replayAlwaysOn != canvasDock->replayAlwaysOn) {
		canvasDock->replayDuration = duration;
		canvasDock->replayPath = replayPath;
		canvasDock->startReplay = startReplay;
		canvasDock->replayAlwaysOn = replayAlwaysOn;
		if (replayAlwaysOn || startReplay) {
			if (obs_output_active(canvasDock->replayOutput)) {
				canvasDock->StopReplayBuffer();
				QTimer::singleShot(500, this, [this] {
					canvasDock->CheckReplayBuffer(true);
				});
			} else {
				canvasDock->CheckReplayBuffer(true);
			}
		} else {
			canvasDock->StopReplayBuffer();
		}
	}

	std::string sk = key->text().toUtf8().constData();
	std::string ss = server->currentText().toUtf8().constData();
	if (sk != canvasDock->stream_key || ss != canvasDock->stream_server) {
		canvasDock->stream_key = sk;
		canvasDock->stream_server = ss;
		if (obs_output_active(canvasDock->streamOutput)) {
			//TODO restart
			//StopStream();
			//StartStream();
		}
	}
	auto sa = !streamingUseMain->isChecked();
	auto se = streamingEncoder->currentData().toString().toUtf8();
	if (canvasDock->stream_advanced_settings != sa ||
	    canvasDock->stream_encoder != se.constData()) {
		canvasDock->stream_advanced_settings = sa;
		canvasDock->stream_encoder = se.constData();
		if (canvasDock->streamOutput &&
		    !obs_output_active(canvasDock->streamOutput)) {
			auto enc = obs_output_get_video_encoder(
				canvasDock->streamOutput);
			obs_output_set_video_encoder(canvasDock->streamOutput,
						     nullptr);
			if (enc &&
			    strcmp(obs_encoder_get_name(enc),
				   "vertical_canvas_video_encoder") == 0) {
				obs_encoder_release(enc);
			}
		}
	}

	for (int i = 1; i <= (int)streamingAudioTracks.size(); i++) {
		if (streamingAudioTracks[i - 1]->isChecked()) {
			if (canvasDock->stream_audio_track != i) {
				if (canvasDock->streamOutput &&
				    !obs_output_active(
					    canvasDock->streamOutput))
					obs_output_set_audio_encoder(
						canvasDock->streamOutput,
						nullptr, 0);
				canvasDock->stream_audio_track = i;
			}
			break;
		}
	}

	obs_data_apply(canvasDock->stream_encoder_settings,
		       stream_encoder_settings);

	canvasDock->recordPath = recordPath->text().toUtf8().constData();

	auto ra = !recordingUseMain->isChecked();
	auto re = recordingEncoder->currentData().toString().toUtf8();

	if (canvasDock->record_advanced_settings != ra ||
	    canvasDock->record_encoder != re.constData()) {

		canvasDock->record_advanced_settings = ra;
		canvasDock->record_encoder = re.constData();

		if ((canvasDock->recordOutput &&
		     !obs_output_active(canvasDock->recordOutput)) ||
		    (canvasDock->replayOutput &&
		     !obs_output_active(canvasDock->replayOutput))) {
			auto enc = canvasDock->recordOutput
					   ? obs_output_get_video_encoder(
						     canvasDock->recordOutput)
					   : nullptr;
			if (!enc && canvasDock->replayOutput) {
				enc = obs_output_get_video_encoder(
					canvasDock->replayOutput);
			}
			if (canvasDock->recordOutput)
				obs_output_set_video_encoder(
					canvasDock->recordOutput, nullptr);
			if (canvasDock->replayOutput)
				obs_output_set_video_encoder(
					canvasDock->replayOutput, nullptr);
			if (enc &&
			    strcmp(obs_encoder_get_name(enc),
				   "vertical_canvas_record_video_encoder") ==
				    0) {
				obs_encoder_release(enc);
			}
		}
	}
	canvasDock->filename_formatting =
		filenameFormat->text().toUtf8().constData();
	canvasDock->file_format =
		fileFormat->currentText().toUtf8().constData();

	long long tracks = 0;
	for (size_t i = 0; i < MAX_AUDIO_MIXES; i++) {
		if (recordingAudioTracks[i]->isChecked())
			tracks += (1ll << i);
	}
	if (!tracks)
		tracks = 1;
	if (canvasDock->record_audio_tracks != tracks) {
		canvasDock->record_audio_tracks = tracks;
		if ((canvasDock->recordOutput &&
		     !obs_output_active(canvasDock->recordOutput)) ||
		    (canvasDock->replayOutput &&
		     !obs_output_active(canvasDock->replayOutput))) {
			for (size_t i = 0; i < MAX_AUDIO_MIXES; i++) {
				auto enc =
					canvasDock->recordOutput
						? obs_output_get_audio_encoder(
							  canvasDock
								  ->recordOutput,
							  i)
						: nullptr;
				if (!enc && canvasDock->replayOutput) {
					enc = obs_output_get_audio_encoder(
						canvasDock->replayOutput, i);
				}
				if (canvasDock->recordOutput)
					obs_output_set_audio_encoder(
						canvasDock->recordOutput,
						nullptr, i);
				if (canvasDock->replayOutput)
					obs_output_set_audio_encoder(
						canvasDock->replayOutput,
						nullptr, i);
				obs_encoder_release(enc);
			}
		}
	}

	obs_data_apply(canvasDock->record_encoder_settings,
		       record_encoder_settings);
}

void OBSBasicSettings::SetEncoderBitrate(obs_encoder_t *encoder)
{
	if (!encoder)
		return;
	auto settings = obs_encoder_get_settings(encoder);
	if (!settings)
		return;
	auto bitrate = obs_encoder_get_type(encoder) == OBS_ENCODER_AUDIO
			       ? canvasDock->audioBitrate
			       : canvasDock->videoBitrate;
	if (obs_data_get_int(settings, "bitrate") == bitrate) {
		obs_data_release(settings);
		return;
	}
	obs_data_set_int(settings, "bitrate", bitrate);
	obs_encoder_update(encoder, nullptr);
	obs_data_release(settings);
}

std::vector<obs_hotkey_t *>
OBSBasicSettings::GetHotKeysFromOutput(obs_output_t *output)
{
	struct find_hotkey {
		std::vector<obs_hotkey_t *> hotkeys;
		obs_weak_output_t *output;
	};
	find_hotkey t = {};
	t.output = obs_output_get_weak_output(output);
	obs_enum_hotkeys(
		[](void *data, obs_hotkey_id id, obs_hotkey_t *key) {
			UNUSED_PARAMETER(id);
			if (obs_hotkey_get_registerer_type(key) !=
			    OBS_HOTKEY_REGISTERER_OUTPUT)
				return true;
			auto hp = (struct find_hotkey *)data;
			auto o = obs_hotkey_get_registerer(key);
			if (o == hp->output ||
			    obs_weak_output_references_output(
				    hp->output, (obs_output_t *)o)) {
				hp->hotkeys.push_back(key);
			}
			return true;
		},
		&t);
	obs_weak_output_release(t.output);
	return t.hotkeys;
}

std::vector<obs_key_combination_t>
OBSBasicSettings::GetCombosForHotkey(obs_hotkey_id hotkey)
{
	struct find_combos {
		obs_hotkey_id hotkey;
		std::vector<obs_key_combination_t> combos;
	};
	find_combos t = {hotkey, {}};
	obs_enum_hotkey_bindings(
		[](void *data, size_t idx, obs_hotkey_binding_t *binding) {
			UNUSED_PARAMETER(idx);
			auto t = (struct find_combos *)data;
			if (t->hotkey ==
			    obs_hotkey_binding_get_hotkey_id(binding)) {
				t->combos.push_back(
					obs_hotkey_binding_get_key_combination(
						binding));
			}
			return true;
		},
		&t);
	return t.combos;
}

std::vector<obs_hotkey_t *>
OBSBasicSettings::GetHotkeyById(obs_hotkey_id hotkey)
{
	struct find_hotkey {
		std::vector<obs_hotkey_t *> hotkeys;
		obs_hotkey_id hotkey_id;
	};
	find_hotkey t = {};
	t.hotkey_id = hotkey;
	obs_enum_hotkeys(
		[](void *data, obs_hotkey_id id, obs_hotkey_t *key) {
			auto hp = (struct find_hotkey *)data;
			if (hp->hotkey_id == id) {
				hp->hotkeys.push_back(key);
			}
			return true;
		},
		&t);
	return t.hotkeys;
}

obs_hotkey_t *OBSBasicSettings::GetHotkeyByName(QString name)
{
	struct find_hotkey {
		obs_hotkey_t *hotkey;
		const char *name;
	};
	find_hotkey t = {};
	auto n = name.toUtf8();
	t.name = n.constData();
	obs_enum_hotkeys(
		[](void *data, obs_hotkey_id id, obs_hotkey_t *key) {
			UNUSED_PARAMETER(id);
			const auto hp = (struct find_hotkey *)data;
			const auto hn = obs_hotkey_get_name(key);
			if (strcmp(hp->name, hn) == 0)
				hp->hotkey = key;
			return true;
		},
		&t);
	return t.hotkey;
}

void OBSBasicSettings::AddProperty(
	obs_property_t *property, obs_data_t *settings, QFormLayout *layout,
	std::map<obs_property_t *, QWidget *> *widgets)
{
	obs_property_type type = obs_property_get_type(property);
	if (type == OBS_PROPERTY_BOOL) {
		auto widget = new QCheckBox(
			QString::fromUtf8(obs_property_description(property)));
		widget->setChecked(obs_data_get_bool(
			settings, obs_property_name(property)));
		layout->addWidget(widget);
		if (!obs_property_visible(property)) {
			widget->setVisible(false);
			int row = 0;
			layout->getWidgetPosition(widget, &row, nullptr);
			auto item = layout->itemAt(row, QFormLayout::LabelRole);
			if (item) {
				auto w = item->widget();
				if (w)
					w->setVisible(false);
			}
		}
		widgets->emplace(property, widget);
		connect(widget, &QCheckBox::stateChanged,
			[this, property, settings, widget, widgets, layout] {
				obs_data_set_bool(settings,
						  obs_property_name(property),
						  widget->isChecked());
				if (obs_property_modified(property, settings)) {
					RefreshProperties(widgets, layout);
				}
			});
	} else if (type == OBS_PROPERTY_INT) {
		auto widget = new QSpinBox();
		widget->setEnabled(obs_property_enabled(property));
		widget->setMinimum(obs_property_int_min(property));
		widget->setMaximum(obs_property_int_max(property));
		widget->setSingleStep(obs_property_int_step(property));
		widget->setValue((int)obs_data_get_int(
			settings, obs_property_name(property)));
		widget->setToolTip(QString::fromUtf8(
			obs_property_long_description(property)));
		widget->setSuffix(
			QString::fromUtf8(obs_property_int_suffix(property)));
		auto label = new QLabel(
			QString::fromUtf8(obs_property_description(property)));
		layout->addRow(label, widget);
		if (!obs_property_visible(property)) {
			widget->setVisible(false);
			label->setVisible(false);
		}
		widgets->emplace(property, widget);
		connect(widget, &QSpinBox::valueChanged,
			[this, property, settings, widget, widgets, layout] {
				obs_data_set_int(settings,
						 obs_property_name(property),
						 widget->value());
				if (obs_property_modified(property, settings)) {
					RefreshProperties(widgets, layout);
				}
			});
	} else if (type == OBS_PROPERTY_FLOAT) {
		auto widget = new QDoubleSpinBox();
		widget->setEnabled(obs_property_enabled(property));
		widget->setMinimum(obs_property_float_min(property));
		widget->setMaximum(obs_property_float_max(property));
		widget->setSingleStep(obs_property_float_step(property));
		widget->setValue(obs_data_get_double(
			settings, obs_property_name(property)));
		widget->setToolTip(QString::fromUtf8(
			obs_property_long_description(property)));
		widget->setSuffix(
			QString::fromUtf8(obs_property_float_suffix(property)));
		auto label = new QLabel(
			QString::fromUtf8(obs_property_description(property)));
		layout->addRow(label, widget);
		if (!obs_property_visible(property)) {
			widget->setVisible(false);
			label->setVisible(false);
		}
		widgets->emplace(property, widget);
		connect(widget, &QDoubleSpinBox::valueChanged,
			[this, property, settings, widget, widgets, layout] {
				obs_data_set_double(settings,
						    obs_property_name(property),
						    widget->value());
				if (obs_property_modified(property, settings)) {
					RefreshProperties(widgets, layout);
				}
			});
	} else if (type == OBS_PROPERTY_TEXT) {
		obs_text_type text_type = obs_property_text_type(property);
		if (text_type == OBS_TEXT_MULTILINE) {
			auto widget = new QPlainTextEdit;
			widget->document()->setDefaultStyleSheet(
				"font { white-space: pre; }");
			widget->setTabStopDistance(40);
			widget->setPlainText(
				QString::fromUtf8(obs_data_get_string(
					settings,
					obs_property_name(property))));
			auto label = new QLabel(QString::fromUtf8(
				obs_property_description(property)));
			layout->addRow(label, widget);
			if (!obs_property_visible(property)) {
				widget->setVisible(false);
				label->setVisible(false);
			}
			widgets->emplace(property, widget);
			connect(widget, &QPlainTextEdit::textChanged,
				[this, property, settings, widget, widgets,
				 layout] {
					obs_data_set_string(
						settings,
						obs_property_name(property),
						widget->toPlainText().toUtf8());
					if (obs_property_modified(property,
								  settings)) {
						RefreshProperties(widgets,
								  layout);
					}
				});
		} else {
			auto widget = new QLineEdit();
			widget->setText(QString::fromUtf8(obs_data_get_string(
				settings, obs_property_name(property))));
			if (text_type == OBS_TEXT_PASSWORD)
				widget->setEchoMode(QLineEdit::Password);
			auto label = new QLabel(QString::fromUtf8(
				obs_property_description(property)));
			layout->addRow(label, widget);
			if (!obs_property_visible(property)) {
				widget->setVisible(false);
				label->setVisible(false);
			}
			widgets->emplace(property, widget);
			if (text_type != OBS_TEXT_INFO) {
				connect(widget, &QLineEdit::textChanged,
					[this, property, settings, widget,
					 widgets, layout] {
						obs_data_set_string(
							settings,
							obs_property_name(
								property),
							widget->text().toUtf8());
						if (obs_property_modified(
							    property,
							    settings)) {
							RefreshProperties(
								widgets,
								layout);
						}
					});
			}
		}
	} else if (type == OBS_PROPERTY_LIST) {
		auto widget = new QComboBox();
		widget->setMaxVisibleItems(40);
		widget->setToolTip(QString::fromUtf8(
			obs_property_long_description(property)));
		auto list_type = obs_property_list_type(property);
		obs_combo_format format = obs_property_list_format(property);

		size_t count = obs_property_list_item_count(property);
		for (size_t i = 0; i < count; i++) {
			QVariant var;
			if (format == OBS_COMBO_FORMAT_INT) {
				long long val =
					obs_property_list_item_int(property, i);
				var = QVariant::fromValue<long long>(val);

			} else if (format == OBS_COMBO_FORMAT_FLOAT) {
				double val = obs_property_list_item_float(
					property, i);
				var = QVariant::fromValue<double>(val);

			} else if (format == OBS_COMBO_FORMAT_STRING) {
				var = QByteArray(obs_property_list_item_string(
					property, i));
			}
			widget->addItem(
				QString::fromUtf8(obs_property_list_item_name(
					property, i)),
				var);
		}

		if (list_type == OBS_COMBO_TYPE_EDITABLE)
			widget->setEditable(true);

		auto name = obs_property_name(property);
		QVariant value;
		switch (format) {
		case OBS_COMBO_FORMAT_INT:
			value = QVariant::fromValue(
				obs_data_get_int(settings, name));
			break;
		case OBS_COMBO_FORMAT_FLOAT:
			value = QVariant::fromValue(
				obs_data_get_double(settings, name));
			break;
		case OBS_COMBO_FORMAT_STRING:
			value = QByteArray(obs_data_get_string(settings, name));
			break;
		default:;
		}

		if (format == OBS_COMBO_FORMAT_STRING &&
		    list_type == OBS_COMBO_TYPE_EDITABLE) {
			widget->lineEdit()->setText(value.toString());
		} else {
			auto idx = widget->findData(value);
			if (idx != -1)
				widget->setCurrentIndex(idx);
		}

		if (obs_data_has_autoselect_value(settings, name)) {
			switch (format) {
			case OBS_COMBO_FORMAT_INT:
				value = QVariant::fromValue(
					obs_data_get_autoselect_int(settings,
								    name));
				break;
			case OBS_COMBO_FORMAT_FLOAT:
				value = QVariant::fromValue(
					obs_data_get_autoselect_double(settings,
								       name));
				break;
			case OBS_COMBO_FORMAT_STRING:
				value = QByteArray(
					obs_data_get_autoselect_string(settings,
								       name));
				break;
			default:;
			}
			int id = widget->findData(value);

			auto idx = widget->currentIndex();
			if (id != -1 && id != idx) {
				QString actual = widget->itemText(id);
				QString selected = widget->itemText(
					widget->currentIndex());
				QString combined = QString::fromUtf8(
					obs_frontend_get_locale_string(
						"Basic.PropertiesWindow.AutoSelectFormat"));
				widget->setItemText(
					idx,
					combined.arg(selected).arg(actual));
			}
		}
		auto label = new QLabel(
			QString::fromUtf8(obs_property_description(property)));
		layout->addRow(label, widget);
		if (!obs_property_visible(property)) {
			widget->setVisible(false);
			label->setVisible(false);
		}
		widgets->emplace(property, widget);
		switch (format) {
		case OBS_COMBO_FORMAT_INT:
			connect(widget, &QComboBox::currentIndexChanged,
				[this, property, settings, widget, widgets,
				 layout] {
					obs_data_set_int(
						settings,
						obs_property_name(property),
						widget->currentData().toInt());
					if (obs_property_modified(property,
								  settings)) {
						RefreshProperties(widgets,
								  layout);
					}
				});
			break;
		case OBS_COMBO_FORMAT_FLOAT:
			connect(widget, &QComboBox::currentIndexChanged,
				[this, property, settings, widget, widgets,
				 layout] {
					obs_data_set_double(
						settings,
						obs_property_name(property),
						widget->currentData()
							.toDouble());
					if (obs_property_modified(property,
								  settings)) {
						RefreshProperties(widgets,
								  layout);
					}
				});
			break;
		case OBS_COMBO_FORMAT_STRING:
			if (list_type == OBS_COMBO_TYPE_EDITABLE) {
				connect(widget, &QComboBox::currentTextChanged,
					[this, property, settings, widget,
					 widgets, layout] {
						obs_data_set_string(
							settings,
							obs_property_name(
								property),
							widget->currentText()
								.toUtf8()
								.constData());
						if (obs_property_modified(
							    property,
							    settings)) {
							RefreshProperties(
								widgets,
								layout);
						}
					});
			} else {
				connect(widget, &QComboBox::currentIndexChanged,
					[this, property, settings, widget,
					 widgets, layout] {
						obs_data_set_string(
							settings,
							obs_property_name(
								property),
							widget->currentData()
								.toString()
								.toUtf8()
								.constData());
						if (obs_property_modified(
							    property,
							    settings)) {
							RefreshProperties(
								widgets,
								layout);
						}
					});
			}
			break;
		default:;
		}
	} else {
		// OBS_PROPERTY_PATH
		// OBS_PROPERTY_COLOR
		// OBS_PROPERTY_BUTTON
		// OBS_PROPERTY_FONT
		// OBS_PROPERTY_EDITABLE_LIST
		// OBS_PROPERTY_FRAME_RATE
		// OBS_PROPERTY_GROUP
		// OBS_PROPERTY_COLOR_ALPHA
	}
	obs_property_modified(property, settings);
}

void OBSBasicSettings::LoadProperty(obs_property_t *prop, obs_data_t *settings,
				    QWidget *widget)
{
	auto type = obs_property_get_type(prop);
	if (type == OBS_PROPERTY_BOOL) {
		((QCheckBox *)widget)
			->setChecked(obs_data_get_bool(
				settings, obs_property_name(prop)));
	} else if (type == OBS_PROPERTY_INT) {
		((QSpinBox *)widget)
			->setValue(obs_data_get_int(settings,
						    obs_property_name(prop)));
	} else if (type == OBS_PROPERTY_FLOAT) {
		((QDoubleSpinBox *)widget)
			->setValue(obs_data_get_double(
				settings, obs_property_name(prop)));
	} else if (type == OBS_PROPERTY_TEXT) {
		obs_text_type text_type = obs_property_text_type(prop);
		if (text_type == OBS_TEXT_MULTILINE) {
			((QPlainTextEdit *)widget)
				->setPlainText(
					QString::fromUtf8(obs_data_get_string(
						settings,
						obs_property_name(prop))));
		} else if (text_type != OBS_TEXT_INFO) {
			((QLineEdit *)widget)
				->setText(QString::fromUtf8(obs_data_get_string(
					settings, obs_property_name(prop))));
		}
	} else if (type == OBS_PROPERTY_LIST) {
		obs_combo_format format = obs_property_list_format(prop);
		auto name = obs_property_name(prop);
		QVariant value;
		switch (format) {
		case OBS_COMBO_FORMAT_INT:
			value = QVariant::fromValue(
				obs_data_get_int(settings, name));
			break;
		case OBS_COMBO_FORMAT_FLOAT:
			value = QVariant::fromValue(
				obs_data_get_double(settings, name));
			break;
		case OBS_COMBO_FORMAT_STRING:
			value = QByteArray(obs_data_get_string(settings, name));
			break;
		default:;
		}

		if (format == OBS_COMBO_FORMAT_STRING &&
		    obs_property_list_type(prop) == OBS_COMBO_TYPE_EDITABLE) {
			((QComboBox *)widget)
				->lineEdit()
				->setText(value.toString());
		} else {
			auto idx = ((QComboBox *)widget)->findData(value);
			if (idx != -1)
				((QComboBox *)widget)->setCurrentIndex(idx);
		}
	} else {
		// OBS_PROPERTY_PATH
		// OBS_PROPERTY_COLOR
		// OBS_PROPERTY_BUTTON
		// OBS_PROPERTY_FONT
		// OBS_PROPERTY_EDITABLE_LIST
		// OBS_PROPERTY_FRAME_RATE
		// OBS_PROPERTY_GROUP
		// OBS_PROPERTY_COLOR_ALPHA
	}
}

void OBSBasicSettings::RefreshProperties(
	std::map<obs_property_t *, QWidget *> *widgets, QFormLayout *layout)
{
	if (widgets == &stream_encoder_property_widgets) {
		obs_property_t *property =
			obs_properties_first(stream_encoder_properties);
		while (property) {
			auto widget = widgets->at(property);
			auto visible = obs_property_visible(property);
			if (widget->isVisible() != visible) {
				widget->setVisible(visible);
				int row = 0;
				layout->getWidgetPosition(widget, &row,
							  nullptr);
				auto item = layout->itemAt(
					row, QFormLayout::LabelRole);
				if (item) {
					widget = item->widget();
					if (widget)
						widget->setVisible(visible);
				}
			}
			obs_property_next(&property);
		}
	} else if (widgets == &record_encoder_property_widgets) {
		obs_property_t *property =
			obs_properties_first(record_encoder_properties);
		while (property) {
			auto widget = widgets->at(property);
			auto visible = obs_property_visible(property);
			if (widget->isVisible() != visible) {
				widget->setVisible(visible);
				int row = 0;
				layout->getWidgetPosition(widget, &row,
							  nullptr);
				auto item = layout->itemAt(
					row, QFormLayout::LabelRole);
				if (item) {
					widget = item->widget();
					if (widget)
						widget->setVisible(visible);
				}
			}
			obs_property_next(&property);
		}
	}
}
