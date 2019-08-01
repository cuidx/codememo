//远程桌面控制命令类型
enum RemoteCtrlCmdType{
	CMD_KEY_UP = 0,						//按键释放
	CMD_KEY_DOWN,							//按键按下
	CMD_MOUSE_LEFT_BUTTON_DOWN,			//鼠标左键按下
	CMD_MOUSE_RIGHT_BUTTON_DOWN,			//鼠标右键按下
	CMD_MOUSE_LEFT_BUTTON_UP,				//鼠标左键释放
	CMD_MOUSE_RIGHT_BUTTON_UP,				//鼠标右键释放
	CMD_MOUSE_LEFT_BUTTON_DOUBLE_CLICK,	//鼠标左键双击
	CMD_MOUSE_MOVE	,						//鼠标移动
	CMD_MOUSE_RESET,						//鼠标位置复位
	CMD_MOUSE_WHEEL,						//鼠标滚轮
};

void SendMouseCtrl(const RemoteCtrlCmdType &type, const QPoint &ptPos,int wheelDelta)
{
	INPUT input;
	memset(&input,0,sizeof(input));
	input.type = INPUT_MOUSE;
	if (type == IVUS_MOUSE_WHEEL)
	{
		input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
		input.mi.dx = 0;
		input.mi.dy = 0;
		input.mi.mouseData = wheelDelta * 120;
		input.mi.time = 0;
		input.mi.dwExtraInfo = 0;
	}
	else
	{
		input.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
		switch (type)
		{
		case CMD_MOUSE_LEFT_BUTTON_DOWN:
			{
				input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
			}
			break;
		case CMD_MOUSE_RIGHT_BUTTON_DOWN:
			{
				input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
			}
			break;
		case CMD_MOUSE_LEFT_BUTTON_UP:
			{
				input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
			}
			break;
		case CMD_MOUSE_RIGHT_BUTTON_UP:
			{
				input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
			}
			break;
		case CMD_MOUSE_LEFT_BUTTON_DOUBLE_CLICK:
			{
				for (int iTime = 0; iTime < 2; iTime++)
				{
					INPUT input0;
					memset(&input0,0,sizeof(input0));
					input0.type = INPUT_MOUSE;
					input0.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;
					input0.mi.dx = ptPos.x() * 1.0 / Width * 65535;
					input0.mi.dy = ptPos.y() * 1.0 / Height * 65535;
					input0.mi.mouseData = 0;
					input0.mi.time = 0;
					input0.mi.dwExtraInfo = 0;
					SendInput(1,&input0,sizeof(INPUT));
					input0.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;
					SendInput(1,&input0,sizeof(INPUT));
										
				}
				return;
			}
			break;
		case CMD_MOUSE_MOVE:
			{
				input.mi.dwFlags |= MOUSEEVENTF_MOVE;
			}
			break;
		default:
			{
				return;
			}
		}
		POINT ptCur;
		::GetCursorPos(&ptCur);
		input.mi.dx = ptPos.x() * 1.0 / Width * 65535;
		input.mi.dy = ptPos.y() * 1.0 / Height * 65535;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dwExtraInfo = 0;
		
	}
	SendInput(1,&input,sizeof(INPUT));
}

void SendKeyboardCtrl(const RemoteCtrlCmdType &type, const int key)
{
	INPUT input;
	memset(&input,0,sizeof(input));
	input.type = INPUT_KEYBOARD;
	if (type == CMD_KEY_UP)
	{
		input.ki.dwFlags |= KEYEVENTF_KEYUP;
	}
	input.ki.wVk = key;
	input.ki.wScan = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1,&input,sizeof(INPUT));
}
