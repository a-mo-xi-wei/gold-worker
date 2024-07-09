#include<iostream>
#include<string>
#include<easyx.h>
#include<cmath>
using namespace std;


class Hook
{
public:
	enum SwingDir
	{
		Left,
		Right
	}swingDir;
	enum State
	{
		Normal,
		Long,
		Short
	}state;
	
public:
	Hook();
	int get_Y();
	int get_X();
	double get_EndX();
	double get_EndY();
	double get_Radian();
	void hook_Draw();//画钩子
	void hook_Swing();//钩子摇摆
	void hook_Control();
	//void hook_Drag(Mine& mine);不能写在钩子处而要写在矿藏处是因为矿藏的坐标为私有成员，无法改变！
private:
	int m_x;
	int m_y;//出发点坐标
	double m_endx;
	double m_endy;//结束坐标
	int m_len;//钩子的长度
	double m_radian;//弧度
	double m_dx;
	double m_dy;//单位时间的长度变化量
	int speed;//速度
};

Hook::Hook()
{
	this->m_len = 50;
	this->m_x = 517;
	this->m_y = 100;
	this->m_endx = this->m_x;
	this->m_endy = this->m_y + this->m_len;
	this->m_radian = 0;
	this->speed = 2;
	swingDir = Right;
	state = Normal;
}

int Hook::get_Y()
{
	return this->m_y;
}

int Hook::get_X()
{
	return this->m_x;
}

double Hook::get_EndX()
{
	return this->m_endx;
}

double Hook::get_EndY()
{
	return this->m_endy;
}

double Hook::get_Radian()
{
	return this->m_radian;
}

void Hook::hook_Draw()
{
	setlinestyle(PS_SOLID, 5);
	setlinecolor(BROWN);
	line(this->m_x, this->m_y, this->m_endx, this->m_endy);
}

void Hook::hook_Swing()
{
	if (this->state == Normal)
	{
		if (this->swingDir == Right)
			this->m_radian += 0.03;
		else if (this->swingDir == Left)
			this->m_radian -= 0.03;
		if (this->m_radian > 80 * 3.14 / 180)
			this->swingDir = Left;
		else if (this->m_radian < -(80 * 3.14 / 180))
			this->swingDir = Right;
		this->m_endx = this->m_x + sin(this->m_radian) * this->m_len;
		this->m_endy = this->m_y + cos(this->m_radian) * this->m_len;
	}
}

void Hook::hook_Control()
{
	if (GetAsyncKeyState(VK_SPACE) && this->state == Normal)
	{
		this->state = Long;
		this->m_dx = sin(m_radian) * this->speed;
		this->m_dy = cos(m_radian) * this->speed;

	}
	if (this->state == Long)
	{
		this->m_endx += this->m_dx;
		this->m_endy += this->m_dy;
	}
	if (this->state == Short)
	{
		this->m_endx -= this->m_dx;
		this->m_endy -= this->m_dy;
		if (sqrt(pow(this->m_x - this->m_endx, 2) + pow(this->m_y - this->m_endy, 2)) <= this->m_len)
			this->state = Normal;
	}
	if (this->m_endx<0 || this->m_endx>getwidth() || this->m_endy > getheight())
		this->state = Short;
}


const int MINE_NUM = 15;//矿藏的数量
//都默认为正方形，就不设置宽高了
const int MINE1SIZE = 40;
const int MINE2SIZE = 50;
const int MONEYSIZE = 30;
const int STONESIZE = 30;
class Mine
{
public:
	Mine();
	void mine_Draw();
	double get_MineX();
	double get_MineY();
	int get_Type();
	int get_Value();
	bool is_Touch(Hook& hook);
	void mine_Drag(Hook& hook);
private:
	double m_x;
	double m_y;//坐标位置
	double m_dx;
	double m_dy;//拖拉变化位置
	int m_size;//金块尺寸
	int m_value;//石头0，钱袋子100，金块200
	int m_type;//金块，钱袋子，石头
	int speed;
	IMAGE m_img[8];
};

Mine::Mine()
{
	this->m_x = rand() % (getwidth() - 100);
	this->m_y = rand() % (getheight() - 180) + 150;
	this->m_type = rand() % 4;//[0,3)四种类型，大小金块，钱袋子，石头
	if (this->m_type == 0)
		this->m_value = 100;
	else if (this->m_type == 1)
		this->m_value = 200;
	else if (this->m_type == 2)
		this->m_value = 400;
	else if (this->m_type == 3)
		this->m_value =50;
	this->speed = 2;
	loadimage(m_img + 0, "./images/0_1.png", MINE1SIZE, MINE1SIZE);
	loadimage(m_img + 1, "./images/0_2.png", MINE1SIZE, MINE1SIZE);
	loadimage(m_img + 2, "./images/0_1.png", MINE2SIZE, MINE2SIZE);
	loadimage(m_img + 3, "./images/0_2.png", MINE2SIZE, MINE2SIZE);
	loadimage(m_img + 4, "./images/1_1.png", MONEYSIZE, MONEYSIZE);
	loadimage(m_img + 5, "./images/1_2.png", MONEYSIZE, MONEYSIZE);
	loadimage(m_img + 6, "./images/2_1.png", STONESIZE, STONESIZE);
	loadimage(m_img + 7, "./images/2_2.png", STONESIZE, STONESIZE);
}

void Mine::mine_Draw()
{
	//受源码启发：四张图是不可能一起绘制的，那么就绘制一张随机的图！

	putimage(this->m_x, this->m_y, m_img + this->m_type * 2, SRCPAINT);
	putimage(this->m_x, this->m_y, m_img + this->m_type * 2 + 1, SRCAND);

}

double Mine::get_MineX()
{
	return this->m_x;
}

double Mine::get_MineY()
{
	return this->m_y;
}

int Mine::get_Type()
{
	return this->m_type;
}

int Mine::get_Value()
{
	return this->m_value;
}

bool Mine::is_Touch(Hook& hook)
{
	if (this->get_Type() == 0)
	{
		if (hook.get_EndX() >=this->get_MineX() && hook.get_EndX() <= this->get_MineX() + MINE1SIZE &&
			hook.get_EndY() >= this->get_MineY() && hook.get_EndY() <= this->get_MineY() + MINE1SIZE)
		{
			hook.state = Hook::Short;
			return true;
		}
	}
	if (this->get_Type() == 1)
	{
		if (hook.get_EndX() >= this->get_MineX() && hook.get_EndX() <= this->get_MineX() + MINE2SIZE &&
			hook.get_EndY() >= this->get_MineY() && hook.get_EndY() <= this->get_MineY() + MINE2SIZE)
		{
			hook.state = Hook::Short;
			return true;
		}
	}
	if (this->get_Type() == 2)
	{
		if (hook.get_EndX() >= this->get_MineX() && hook.get_EndX() <= this->get_MineX() + MONEYSIZE &&
			hook.get_EndY() >= this->get_MineY() && hook.get_EndY() <= this->get_MineY() + MONEYSIZE)
		{
			hook.state = Hook::Short;
			return true;
		}
	}
	if (this->get_Type() == 3)
	{
		if (hook.get_EndX() >= this->get_MineX() && hook.get_EndX() <= this->get_MineX() + STONESIZE &&
			hook.get_EndY() >= this->get_MineY() && hook.get_EndY() <= this->get_MineY() + STONESIZE)
		{
			hook.state = Hook::Short;
			return true;
		}
	}
	return false;
}

void Mine::mine_Drag(Hook& hook)
{
	this->m_dx = this->speed * sin(hook.get_Radian());
	this->m_dy = this->speed * cos(hook.get_Radian());
	if (this->get_MineY() > hook.get_Y())//限制移动位置
	{
		this->m_x -= this->m_dx;
		this->m_y -= this->m_dy;
		
	}
	else
	{
		this->m_x = hook.get_X();
		this->m_y = hook.get_Y()-150;//拉倒身边后需要让图片消失,即从视野上消失
		hook.state = Hook::Normal;
		
	}
}

void drawGrade(int &sum, int& grade, Mine(&mine)[MINE_NUM], Hook& hook)
{
	setfillcolor(RGB(235, 255, 139));
	solidrectangle(getwidth() - 200, 0, getwidth(), 120);
	settextcolor(RED);
	settextstyle(30, 0, "楷体");
	outtextxy(getwidth() - 180, 10, "总分：");
	outtextxy(getwidth() - 180, 70, "得分：");

	//计算总分
	sum = 0;
	for (int i = 0; i < MINE_NUM; i++)
	{
		sum+=mine[i].get_Value();
	}
	outtextxy(getwidth() - 100, 10,to_string(sum).c_str());
	//计算得分
	grade = 0;
	for (int i = 0; i < MINE_NUM; i++)
	{
		if (mine[i].get_MineX() == hook.get_X() && mine[i].get_MineY() == hook.get_Y() - 150)
			grade += mine[i].get_Value();
	}
	outtextxy(getwidth() - 100, 70, to_string(grade).c_str());
}
int main()
{
	initgraph(1080, 640,EX_SHOWCONSOLE);
	setbkmode(TRANSPARENT);
	srand((unsigned int)time(NULL));
	Hook hook;
	Mine mine[MINE_NUM];
	IMAGE img_bk;
	loadimage(&img_bk, "./images/bk.jpg",getwidth(),getheight()-120);

	IMAGE img_oldMan_down[2];
	loadimage(img_oldMan_down+0, "./images/oldMan_down1.png");
	loadimage(img_oldMan_down+1, "./images/oldMan_down2.png");

	IMAGE img_oldMan_up[2];
	loadimage(img_oldMan_up+0, "./images/oldMan_up1.png");
	loadimage(img_oldMan_up+1, "./images/oldMan_up2.png");
	static int sum = 0;//总分
	static int grade = 0;//得分
	int flag = false;//未碰到矿藏
	while (true)
	{
		BeginBatchDraw();
		cleardevice();
		putimage(0, 120, &img_bk);
		setfillcolor(RGB(200, 142, 101));
		solidrectangle(0, 0, getwidth(), 120);
		drawGrade(sum,grade,mine,hook);
		putimage((getwidth() - img_oldMan_down->getwidth()) / 2, 0, img_oldMan_down + 0, SRCPAINT);
		putimage((getwidth() - img_oldMan_down->getwidth()) / 2, 0, img_oldMan_down + 1, SRCAND);
		hook.hook_Draw();
		for (int i = 0; i < MINE_NUM; i++)
		{
			mine[i].mine_Draw();
		}
		hook.hook_Swing();
		hook.hook_Control();
		for (int i = 0; i < MINE_NUM; i++)//检测当前那个矿被钩子碰到
		{
			flag = mine[i].is_Touch(hook);
			if (flag)
				mine[i].mine_Drag(hook);
		} 
		
		EndBatchDraw();
		Sleep(10); 
	}
	return 0;
}

