struct Drive
{
	String DriveName;
	String DriveType;
	String NameBuffer;
	String FileSystem;
	String VSNumber;


	Drive(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		DriveName = req->GetValue("DriveName")->Value().c_str();
		NameBuffer = req->GetValue("NameBuffer")->Value().c_str();
		FileSystem = req->GetValue("FileSystem")->Value().c_str();
		VSNumber = req->GetValue("VSNumber")->Value().c_str();

	};

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("DriveName", DriveName);
		req->AddPair("DriveType", DriveType);
		req->AddPair("NameBuffer", NameBuffer);
		req->AddPair("FileSystem", FileSystem);
		req->AddPair("VSNumber", VSNumber);
		return req->ToString();
	};

};

struct MasDrive {
	vector<Drive>arr;

	String ToJson() {
		auto tree = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		tree->AddPair("mas", mas);
		return tree->ToString();
	};

	MasDrive(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(Drive(mas->Items[i]->ToString()));
		}
	};

	MasDrive() {
		arr.clear();
	};

};

MasDrive GetInfo(){
	MasDrive drives;
	char vol_name[MAX_PATH + 1];
	HANDLE find_handle;
	find_handle = FindFirstVolume(vol_name, ARRAYSIZE(vol_name));
	int k = 0;
	while (FindNextVolume(find_handle, vol_name, ARRAYSIZE(vol_name))) {
		// cout << vol_name << endl;
		char path[MAX_PATH + 1];
		DWORD lp;
		if (GetVolumePathNamesForVolumeName(vol_name, path,
			ARRAYSIZE(vol_name), &lp)) {
			Drive n;
			char NameBuffer[MAX_PATH];
			char SysNameBuffer[MAX_PATH];
			DWORD VSNumber;
			DWORD MCLength;
			DWORD FileSF;
			n.DriveName = String(path);
			if (GetVolumeInformation(path,NameBuffer, sizeof(NameBuffer),
				&VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer)))
			{
				(n.NameBuffer = String(NameBuffer),n.FileSystem = String(SysNameBuffer),n.VSNumber = String(VSNumber));
			}
			switch (GetDriveType(path)){
				case DRIVE_NO_ROOT_DIR:
					n.DriveType = "не найден";
					break;
				case DRIVE_REMOVABLE:
					n.DriveType = "извлекаемый диск";
					break;
				case DRIVE_FIXED:
					n.DriveType = "жесткий диск";
					break;
				case DRIVE_REMOTE:
					n.DriveType = "сетевой диск";
					break;
				case DRIVE_CDROM:
					n.DriveType = "СD ROM";
					break;
				case DRIVE_RAMDISK:
					n.DriveType = "в оперативной памяти";
					break;				
				default:
					n.DriveType = "неизвестный тип";
					break;
			}

			drives.mas.push_back(n);
		}
	}
	return drives;
}