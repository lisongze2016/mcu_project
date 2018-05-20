#include "include/config.h"
#include "include/pdiusbd12.h"
#include "include/uart.h"
#include "include/usbcore.h"
#include "include/ch9.h"
#include <string.h>

idata uint8 Buffer[16];  	//读端点0用的缓冲区
idata uint8 UsbMouse_ConfigDesc_buf[128];
uint8 * pSendData;				//当前发送数据的位置
uint16 SendLength;				//需要发送数据的长度
//是否需要发送0数据包的标志。在USB控制传输的数据过程中，
//当返回的数据包字节数少于最大包长时，会认为数据过程结束。
//当请求的字节数比实际需要返回的字节数长，而实际返回的字节
//数又刚好是端点0大小的整数倍时，就需要返回一个0长度的数据包
//来结束数据过程。因此这里增加一个标志，供程序决定是否需要返回
//一个0长度的数据包。
uint8 NeedZeroPacket;

code struct usb_device_descriptor DeviceDescriptor = {
#if 0
	.bLength 						= USB_DT_DEVICE_SIZE,
	.bDescriptorType 		= 0x01,
	.bcdUSB 						= swab16(0x0110),
	.bDeviceClass				= 0,
	.bDeviceSubClass		= 0,
	.bDeviceProtocol		= 0,
	.bMaxPacketSize0		= 16,
	.idVendor						= swab16(0x8888),
	.idProduct					= swab16(0x0001),
	.bcdDevice					= swab16(0x0100),
	.iManufacturer			= 1,
	.iProduct						= 2,
	.iSerialNumber			= 3,
	.bNumConfigurations = 1,
#endif

	//bLength 该描述符的长度，设备描述符长度为18字节
	USB_DT_DEVICE_SIZE,
	//bDescriptorType 该描述符的类型，设备描述符的类型编号为0x01
	0x01,
	//bcdUSB 该设备所使用的USB协议版本，小端结构(__le16),USB版本USB1.1，即0x0110，低字节在前0x1001
	swab16(0x0110),
	//bDeviceClass 设备所使用的类代码，对于大多数标准的设备类，该字段设为0，bDeviceClass为0时，bDeviceSubClass也必须为0，bDeviceClass为0xff时，表示厂商自定义设备类
	0,
	//bDeviceSubClass 设备使用的子类代码
	0,
	//bDeviceProtocol 设备使用的协议代码，协议代码由USB协会规定，0：设备不使用类所定义的协议，FF：厂商自定义协议
	0,
	//bMaxPacketSize0 端点0的最大包长，可取值8，16，32，64
	16,
	//idVendor 厂商ID号，该ID号由USB协会分配，小端结构(__le16),这里设置为0x8888
	swab16(0x8888),
	//idProduct 产品ID号，小端结构(__le16),0x0001
	swab16(0x0001),
	//bcdDevice 设备版本号，小端结构(__le16),v1.0 0x0100
	swab16(0x0100),
	//iManufacturer 厂商字符串的索引值1
	1,
	//iProduct 产品字符串的索引值2
	2,
	//iSerialNumber 设备序列号字符串的索引值3
	3,
	//bNumConfigurations 表示设备有多少种配置，每种配置都会有一个配置描述符，主机通过设置配置来选择某一种配置，大部分USB设置只有一种配置，即该字段为1
	1
};

struct UsbMouse_ConfigDescriptor UsbMouse_ConfigDesc;

code struct usb_config_descriptor usbConfigDescriptor = {
#if 0
	.bLength = USB_DT_CONFIG_SIZE,
	.bDescriptorType = 0x02,
	.wTotalLength = swab16(???),
	.bNumInterfaces = 0x01,			//鼠标功能单一只具有一个接口
	.bConfigurationValue = 0x01,	//该配置的值
	.iConfiguration = 0x00,			//描述该配置的字符串的索引值
	.bmAttributes = USB_CONFIG_ATT_ONE,	//该设备的属性,总线供电
	.bMaxPower = 0x32,					//设备所需要的电流(单位为2mA)，100mA
#endif
	USB_DT_CONFIG_SIZE,
	0x02,
	swab16(USB_DT_CONFIG_SIZE+USB_DT_INTERFACE_SIZE+USB_DT_HID_SIZE+USB_DT_ENDPOINT_SIZE),//swab16(34),
	0x01,
	0x01,
	0x00,
	USB_CONFIG_ATT_ONE,
	0x32,
};

code struct usb_interface_descriptor usbInterfaceDescriptor = {
#if 0
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = 0x04,
	.bInterfaceNumber = 0x00,
	.bAlternateSetting = 0x00,
	.bNumEndpoints = 0x01;		//非0端点的数目，usb鼠标只需要一个中断输入端点
	.bInterfaceClass = 0x03,	//该接口所使用的类，usb鼠标是HID类，HID类编码是0x03
	.bInterfaceSubClass = 0x01,	//该接口所使用的子类，引导启动的子类
	.bInterfaceProtocol = 0x02,	//该接口所使用的协议,鼠标的代码是0x02
	.iInterface = 0x00;			//描述该接口的字符串的索引值,没有，为0
#endif
	USB_DT_INTERFACE_SIZE,
	0x04,
	0x00,
	0x00,
	0x01,
	0x03,
	0x01,
	0x02,
	0x00,
};

code uint8 ReportDescriptor[]= {0x00};

code struct hid_descriptor HidDescriptor = {
#if 0
	.bLength			= 0x09,
	.bDescriptorType	= 0x21,
	.bcdHID				= 0x0110,
	.bCountryCode		= 0x21,//美式
	.bNumDescriptors	= 0x01,
	.desc[1].bDescriptorType = 0x22,//下级描述符类型为报告描述符，编号为0x22
	.desc[1].wDescriptorLength = swab16(sizeof(ReportDescriptor)),
#endif
	0x09,
	0x21,
	swab16(0x0110),
	0x21,
	0x01,
	0x22,
	swab16(sizeof(ReportDescriptor)),
};
code struct usb_endpoint_descriptor EndpointDescriptor = {

#if 0
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = 0x05,

	.bEndpointAddress = 0x81,		//该端点的地址	D7:该端点的传输方向(1:输入 0:输出) D6~D4:保留位 D3~D0:端点号
	.bmAttributes = 0x03,			//该端点的属性 D1~D0:传输类型(0:控制传输 1:等时传输 2:批量传输 3:中断传输)，
								//如果该端点为非等时传输,D7~D2为保留位，设为0
								//如果该端点为等时传输，D3~D2:同步类型(0:无同步 1:异步 2:适配 3:同步) D5~D4:用途(0:数据端点 1:反馈端点 2:暗含反馈的数据端点 3:保留位) D7~D6为保留位
	.wMaxPacketSize = 0x0010,		//该端点支持的最大包长度,小端结构(__le16)
								//对于全速和低速模式，D10~D0表示端点的最大包长
								//对于高速模式，D12~D10表示每个帧附加的传输次数
	.bInterval = 0x0a,			//端点的查询时间，对于中断端点，表示查询的帧间隔数
#endif
	USB_DT_ENDPOINT_SIZE,
	0x05,
	0x81,	//D12 输入端点1
	0x03,	//中断传输
	swab16(0x0010),	//端点1的最大包长为16字节
	0x0a,	//端点查询时间，设置为10个帧时间，即10ms
};

/************************语言ID的定义********************/
code uint8 LanguageId[4]=
{
	0x04, //本描述符的长度
	0x03, //字符串描述符
	//0x0409为美式英语的ID
	0x09,
	0x04
};

//厂商字符串
//8位小端格式
code uint8 ManufacturerStringDescriptor[82]={
	46, 		//该描述符的长度为46字节
	0x03,		//字符串描述符的类型编码为0x03
	0x68, 0x00, //h
	0x74, 0x00, //t
	0x74, 0x00, //t
	0x70, 0x00, //p
	0x3a, 0x00, //:
	0x2f, 0x00, ///
	0x2f, 0x00, ///
	0x77, 0x00, //w
	0x77, 0x00, //w
	0x77, 0x00, //w
	0x2e, 0x00, //.
	0x6c, 0x00, //l
	0x69, 0x00, //i
	0x73, 0x00, //s
	0x6f, 0x00, //o
	0x6e, 0x00, //n
	0x67, 0x00, //g
	0x7a, 0x00, //z
	0x65, 0x00, //e
	0x2e, 0x00, //.
	0x63, 0x00, //c
	0x6e, 0x00  //n
};

//产品字符串 Unicode编码
//8位小端格式
code uint8 ProductStringDescriptor[34]={
	24,         //该描述符的长度为24字节
	0x03,       //字符串描述符的类型编码为0x03
	0x75, 0x00, //u
	0x73, 0x00, //s
	0x62, 0x00, //b
	0x20, 0x9f, //鼠
	0x07, 0x68, //标
	0x0c, 0xff, //，
	0x62, 0x00, //b
	0x79, 0x00, //y
	0x4e, 0x67, //李
	0x7e, 0x67, //松
	0xfd, 0x6c  //泽
};

//产品序列号字符串 Unicode编码
//8位小端格式
code uint8 SerialNumberStringDescriptor[22]={
	22, 		//该描述符的长度为22字节
	0x03,		//字符串描述符的类型编码为0x03
	0x32, 0x00, //2
	0x30, 0x00, //0
	0x31, 0x00, //1
	0x38, 0x00, //8
	0x2d, 0x00, //-
	0x30, 0x00, //0
	0x35, 0x00, //5
	0x2d, 0x00, //-
	0x32, 0x00, //2
	0x30, 0x00  //0
};

/********************************************************************
函数功能：延时x毫秒函数。
入口参数：x：延时的毫秒数。
返    回：无。
备    注：无。
********************************************************************/
void DelayXms(uint16 x)                
{
	uint16 i;
	uint16 j;
	for(i=0;i<x;i++)
		for(j=0;j<227;j++); //循环语句延时
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：USB断开连接函数。
入口参数：无。
返    回：无。
备    注：无。

Set Mode命令：0xF3
数据：写2个字节 配置字节和时钟分频系数字节
配置字节：
	bit 0：保留 置0
	bit 1：	无懒时钟(低频时钟)模式
					[1] 表示 CLKOUT 不会切换到 LazyClock 
					0 表示 CLKOUT 在 Suspend 脚变高之后切换到 LazyClock 
						LazyClock 频率是 30KHz ± 40% 编程值将不过会被总线复位所改变
	bit 2：	时钟运行
					[1] 表示内部时钟和 PLL 即使在挂起状态下仍然运行 
					0 表示只要不需要时 内部时钟晶振和 PLL 就停止运行 
						为了满足严格的挂起电流要求 该位需要设置为 0 已编程的值不会被总线复位所改变
	bit 3：	中断模式
					1 表示报告所有的错误和 NAKing 并产生一个中断 
					[0] 表示只有 OK 被报告 编程值不会被总线复位所改变
	bit 4：	软连接控制
					1 表示如果 VBUS 可用上行数据上拉电阻就被连接 
					[0] 表示不连接 已编程的值不会被总线复位所改变
	bit 5：	RESERVED; WRITE 0
	bit 7~6：	端点配置选择
					模式 [0] 非同步模式  
					模式 1 同步输出模式  
					模式 2 同步输入模式  
					模式 3 同步输入 / 输出模式
	为方便调试，不考虑省电，时钟都使能，中断模式选择正确传输才产生中断
	端点模式选择为模式0，即端点1，2都为普通端点，因为这里不需要等时传输
	0x06
时钟分频系数字节:
	bit 3~0:	时钟分频系数
						该值用来表示 CLKOUT 的时钟分频系数 用 N 表示分频系数 那么输出频率就为
						48MHz/(N+1) 复位值为 11 这产生 4MHz 的输出频率 然后可由用户自行调节
						当N 为 0 时 得到最大频率 48MHz 当 N 取最大 11 时 得到最小频率 4MHz 
						PDIUSBD12的设计确保了在改变频率时不会出现干扰 已编程的值不会被总线复位所改变
	bit 5~4		RESERVED; WRITE 0
	bit 6:		SET_TO_ONE  
						该位需要在任何 DMA 读或写操作之前置为 1 
						该位在上电复位值为 0 复位后可将其一直设为 1  
	bit 7:		仅有 SOF 中断模式
						将该位置 1 后 仅当帧时钟的起始 SOF 时刻引起中断的产生
						而不管引脚中断模式的设置状态 设置 DMA 位 5
	设置为8分频 clkout端口输出6MHZ 0111
	中断在任何时刻产生						 0100
	0x47
********************************************************************/
void UsbDisconnect(void)
{
#ifdef DEBUG0
	Prints("断开USB连接。\r\n");
#endif
	D12WriteCommand(D12_SET_MODE);  //写设置模式命令
	D12WriteByte(0x06); //设置模式的第一字节	D- 上拉电阻断开
	D12WriteByte(0x47); //设置模式的第二字节
	DelayXms(1000);  //延迟1秒
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：USB连接函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbConnect(void)
{
#ifdef DEBUG0
	Prints("连接USB。\r\n");
#endif
	D12WriteCommand(D12_SET_MODE);  //写设置模式命令
	D12WriteByte(0x16); //设置模式的第一字节	D- 上拉电阻接上
	D12WriteByte(0x47); //设置模式的第二字节
}

/********************************************************************
函数功能：总线挂起中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbBusSuspend(void)
{
#ifdef DEBUG0
 Prints("USB总线挂起。\r\n");
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：总线复位中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbBusReset(void)
{
#ifdef DEBUG0
 Prints("USB总线复位。\r\n");
#endif
 Ep1InIsBusy=0; //复位后端点1输入缓冲区空闲。
}
////////////////////////End of function//////////////////////////////



/********************************************************************
函数功能：根据pData和SendLength将数据发送到端点0的函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp0SendData(void)
{
	//将数据写到端点中去准备发送
	//写之前要先判断一下需要发送的数据是否比端点0
	//最大长度大，如果超过端点大小，则一次只能发送
	//最大包长的数据。端点0的最大包长在DeviceDescriptor.bMaxPacketSize0
	if(SendLength > DeviceDescriptor.bMaxPacketSize0) {
		//按最大包长度发送
		D12WriteEndpointBuffer(1,DeviceDescriptor.bMaxPacketSize0,pSendData);
		//发送后剩余字节数减少最大包长
		SendLength -= DeviceDescriptor.bMaxPacketSize0;
		//发送一次后指针位置要调整
		pSendData += DeviceDescriptor.bMaxPacketSize0;
	} else {
		if(SendLength != 0) {
			//不够最大包长，可以直接发送
			D12WriteEndpointBuffer(1,SendLength,pSendData);
			//发送完毕后，SendLength长度变为0
			SendLength=0;
		} else { //如果要发送的数据包长度为0
			if(NeedZeroPacket==1) {//如果需要发送0长度数据
				D12WriteEndpointBuffer(1,0,pSendData); //发送0长度数据包
				NeedZeroPacket=0; //清需要发送0长度数据包标志
			}
		}
	}
}
////////////////////////End of function//////////////////////////////

struct usb_ctrlrequest request;

void parse_request(char *Buffer)
{
	//将缓冲数据填到设备请求的各字段中
	request.bmRequestType=Buffer[0];
	request.bRequest=Buffer[1];
	request.wValue=Buffer[2]+(((uint16)Buffer[3])<<8);
	request.wIndex=Buffer[4]+(((uint16)Buffer[5])<<8);
	request.wLength=Buffer[6]+(((uint16)Buffer[7])<<8);

	//下面的代码判断具体的请求，并根据不同的请求进行相关操作
  //如果D7位为1，则说明是输入请求
  if((request.bmRequestType&0x80)==0x80){
		//根据bmRequestType的D6~5位散转，D6~5位表示请求的类型
		//0为标准请求，1为类请求，2为厂商请求。
		switch((request.bmRequestType>>5)&0x03){
			case 0:  //标准请求
				#ifdef DEBUG0
				Prints("USB标准输入请求：");
				#endif
				//USB协议定义了几个标准输入请求，我们实现这些标准请求即可
				//请求的代码在bRequest中，对不同的请求代码进行散转
				//事实上，我们还需要对接收者进行散转，因为不同的请求接收者
				//是不一样的。接收者在bmRequestType的D4~D0位中定义。
				//我们这里为了简化操作，有些就省略了对接收者的判断。
				//例如获取描述符的请求，只根据描述符的类型来区别。
				switch(request.bRequest){
					case USB_REQ_GET_CONFIGURATION: //获取配置
						#ifdef DEBUG0
						Prints("获取配置。\r\n");
						#endif
						break;
					case USB_REQ_GET_DESCRIPTOR:  //获取描述符
						#ifdef DEBUG0
						Prints("获取描述符——");
						#endif
						//对描述符类型进行散转，对于全速设备和低速设备，
						//标准请求只支持发送到设备的设备、配置、字符串三种描述符
						//wValue域的第一个字节(低字节)表示索引号，用来选择同一种描述符中的具体某个描述符
						//第二个字节表示描述符的类型编号
						switch((request.wValue>>8)&0xFF){
							case DEVICE_DESCRIPTOR: //设备描述符
								#ifdef DEBUG0
								Prints("设备描述符。\r\n");
								#endif
								pSendData=(uint8 *)&DeviceDescriptor;  //需要发送的数据
								//判断请求的字节数是否比实际需要发送的字节数多
								//这里请求的是设备描述符，因此数据长度就是
								//DeviceDescriptor[0]。如果请求的比实际的长，
								//那么只返回实际长度的数据
								if(request.wLength>DeviceDescriptor.bLength){
									SendLength = DeviceDescriptor.bLength;
									if(SendLength%DeviceDescriptor.bMaxPacketSize0 == 0) { //并且刚好是整数个数据包时
										NeedZeroPacket=1; //需要返回0长度的数据包
									}
								} else {
									SendLength = request.wLength;
								}
								//将数据通过EP0返回
								UsbEp0SendData();
								break;
							case CONFIGURATION_DESCRIPTOR:  //配置描述符
								#ifdef DEBUG0
								Prints("配置描述符。\r\n");
								#endif

								UsbMouse_ConfigDesc.usb_config_desc = &usbConfigDescriptor;
								UsbMouse_ConfigDesc.usb_interface_desc = &usbInterfaceDescriptor;
								UsbMouse_ConfigDesc.hid_desc = &HidDescriptor;
								UsbMouse_ConfigDesc.usb_endpoint_desc = &EndpointDescriptor;
								memcpy(UsbMouse_ConfigDesc_buf,UsbMouse_ConfigDesc.usb_config_desc,USB_DT_CONFIG_SIZE);
								memcpy(UsbMouse_ConfigDesc_buf+USB_DT_CONFIG_SIZE,UsbMouse_ConfigDesc.usb_interface_desc,USB_DT_INTERFACE_SIZE);
								memcpy(UsbMouse_ConfigDesc_buf+USB_DT_CONFIG_SIZE+USB_DT_INTERFACE_SIZE,UsbMouse_ConfigDesc.hid_desc,USB_DT_HID_SIZE);
								memcpy(UsbMouse_ConfigDesc_buf+USB_DT_CONFIG_SIZE+USB_DT_INTERFACE_SIZE+USB_DT_HID_SIZE,UsbMouse_ConfigDesc.usb_endpoint_desc,USB_DT_ENDPOINT_SIZE);

								pSendData = (uint8 *)&UsbMouse_ConfigDesc_buf; //需要发送的数据为配置描述符
								//判断请求的字节数是否比实际需要发送的字节数多
								//这里请求的是配置描述符集合，因此数据长度就是
								//如果请求的比实际的长，那么只返回实际长度的数据
								SendLength = swab16(UsbMouse_ConfigDesc.usb_config_desc->wTotalLength);
								//PrintHex(sizeof(UsbMouse_ConfigDesc));

								if(request.wLength > SendLength) {
									if(SendLength%DeviceDescriptor.bMaxPacketSize0 == 0) {//并且刚好是整数个数据包时
										NeedZeroPacket = 1; //需要返回0长度的数据包
									}
								} else {
									SendLength = request.wLength;
								}
								//将数据通过EP0返回
								UsbEp0SendData();
								break;
							case STRING_DESCRIPTOR:  //字符串描述符
								#ifdef DEBUG0
								Prints("字符串描述符。\r\n");
								#endif
								//根据wValue的低字节（索引值）散转
								switch(request.wValue&0xFF) {
									case 0:  //获取语言ID
										#ifdef DEBUG0
										Prints("(语言ID)。\r\n");
										#endif
										pSendData = LanguageId;
										SendLength = LanguageId[0];
										break;
									case 1:  //厂商字符串的索引值为1，所以这里为厂商字符串
										#ifdef DEBUG0
										Prints("(厂商描述)。\r\n");
										#endif
										pSendData = ManufacturerStringDescriptor;
										SendLength = ManufacturerStringDescriptor[0];
										break;
									 case 2:  //产品字符串的索引值为2，所以这里为产品字符串
										#ifdef DEBUG0
										Prints("(产品描述)。\r\n");
										#endif
										pSendData = ProductStringDescriptor;
										//SendLength = ProductStringDescriptor[0];
										break;
									case 3:  //产品序列号的索引值为3，所以这里为序列号
										#ifdef DEBUG0
										Prints("(产品序列号)。\r\n");
										#endif
										pSendData = SerialNumberStringDescriptor;
										SendLength = SerialNumberStringDescriptor[0];
										break;
									default :
										#ifdef DEBUG0
										Prints("(未知的索引值)。\r\n");
										#endif
										//对于未知索引值的请求，返回一个0长度的包
										SendLength = 0;
										NeedZeroPacket = 1;
										break;
								}
								if(request.wLength > SendLength) {
									if(SendLength%DeviceDescriptor.bMaxPacketSize0 == 0) {//并且刚好是整数个数据包时
										NeedZeroPacket = 1; //需要返回0长度的数据包
									}
								} else {
									SendLength = request.wLength;
								}
								//将数据通过EP0返回
								UsbEp0SendData();
								break;
							default:  //其它描述符
								#ifdef DEBUG0
								Prints("其他描述符，描述符代码：");
								PrintHex((request.wValue>>8)&0xFF);
								Prints("\r\n");
								#endif
								break;
						}
						break;
					case USB_REQ_GET_INTERFACE: //获取接口
						#ifdef DEBUG0
						Prints("获取接口。\r\n");
						#endif
						break;
					case USB_REQ_GET_STATUS: //获取状态
						#ifdef DEBUG0
						Prints("获取状态。\r\n");
						#endif
						break;
					case USB_REQ_SYNCH_FRAME: //同步帧
						#ifdef DEBUG0
						Prints("同步帧。\r\n");
						#endif
						break;
					default:  //未定义的标准请求
						#ifdef DEBUG0
						Prints("错误：未定义的标准输入请求。\r\n");
						#endif
						break;
				}
				break;
			case 1:  //类请求
				#ifdef DEBUG0
				Prints("USB类输入请求：\r\n");
				#endif
				break;
			case 2:  //厂商请求
				#ifdef DEBUG0
				Prints("USB厂商输入请求：\r\n");
				#endif
				break;
			default: //未定义的请求。这里只显示一个报错信息。
				#ifdef DEBUG0
				Prints("错误：未定义的输入请求。\r\n");
				#endif
				break;
		}
	} else {		//否则说明是输出请求
		//根据bmRequestType的D6~5位散转，D6~5位表示请求的类型
		//0为标准请求，1为类请求，2为厂商请求。
		switch((request.bmRequestType>>5)&0x03) {
			case 0:  //标准请求
				#ifdef DEBUG0
				Prints("USB标准输出请求：");
				#endif
				//USB协议定义了几个标准输出请求，我们实现这些标准请求即可
				//请求的代码在bRequest中，对不同的请求代码进行散转
				switch(request.bRequest) {
					case USB_REQ_CLEAR_FEATURE: //清除特性
						#ifdef DEBUG0
						Prints("清除特性。\r\n");
						#endif
						break;
					case USB_REQ_SET_ADDRESS:  //设置地址
						#ifdef DEBUG0
						Prints("设置地址。地址为：");
						PrintHex(request.wValue&0xFF); //显示所设置的地址
						Prints("\r\n");
						#endif
						D12SetAddress(request.wValue&0xFF); //wValue中的低字节是设置的地址值
						//设置地址没有数据过程，直接进入到状态过程，返回一个0长度的数据包
						SendLength = 0;
						NeedZeroPacket = 1;
						//将数据通过EP0返回
						UsbEp0SendData();
						break;
					case USB_REQ_SET_CONFIGURATION: //设置配置
						#ifdef DEBUG0
						Prints("设置配置。\r\n");
						#endif
						break;
					case USB_REQ_SET_DESCRIPTOR: //设置描述符
						#ifdef DEBUG0
						Prints("设置描述符。\r\n");
						#endif
						break;

					case USB_REQ_SET_FEATURE: //设置特性
						#ifdef DEBUG0
						Prints("设置特性。\r\n");
						#endif
						break;

					case USB_REQ_SET_INTERFACE: //设置接口
						#ifdef DEBUG0
						Prints("设置接口。\r\n");
						#endif
						break;

					default:  //未定义的标准请求
						#ifdef DEBUG0
						Prints("错误：未定义的标准输出请求。\r\n");
						#endif
						break;
				}
				break;
			case 1:  //类请求
				#ifdef DEBUG0
				Prints("USB类输出请求：");
				#endif
				break;
			case 2:  //厂商请求
				#ifdef DEBUG0
				Prints("USB厂商输出请求：\r\n");
				#endif
				break;

			default: //未定义的请求。这里只显示一个报错信息。
				#ifdef DEBUG0
				Prints("错误：未定义的输出请求。\r\n");
				#endif
				break;
		}
	}
}

/********************************************************************
函数功能：端点0输出中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp0Out(void)
{
#ifdef DEBUG0
	Prints("USB端点0输出中断。OUT(Host -> Device)\r\n");
	//读取端点0输出最后传输状态，该操作清除中断标志
	//并判断第5位是否为1，如果是，则说明是建立包
	if(D12ReadEndpointLastStatus(0)&0x20)
	{
		D12ReadEndpointBuffer(0,16,Buffer); //读建立过程数据
		D12AcknowledgeSetup(); //应答建立包
		D12ClearBuffer(); //清缓冲区

		//解析主机对设备的请求
		parse_request(Buffer);
	}
	else		//普通数据输出
	{
		D12ReadEndpointBuffer(0,16,Buffer);
		D12ClearBuffer();
	}
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：端点0输入中断处理函数。
入口参数：无。
返    回：无。
备    注：当数据成功发送后，端点0输入中断就会产生，在这里发送剩下的数据
********************************************************************/
void UsbEp0In(void)
{
#ifdef DEBUG0
	Prints("USB端点0输入中断。IN(Device -> Host)\r\n");
#endif
	//读最后发送状态，这将清除端点0的中断标志位
	D12ReadEndpointLastStatus(1);
	//发送剩余的字节数
	UsbEp0SendData();
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：端点1输出中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp1Out(void)
{
#ifdef DEBUG0
 Prints("USB端点1输出中断。\r\n");
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：端点1输入中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp1In(void)
{
#ifdef DEBUG0
 Prints("USB端点1输入中断。\r\n");
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：端点2输出中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp2Out(void)
{
#ifdef DEBUG0
 Prints("USB端点2输出中断。\r\n");
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：端点2输入中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UsbEp2In(void)
{
#ifdef DEBUG0
 Prints("USB端点2输入中断。\r\n");
#endif
}
////////////////////////End of function//////////////////////////////
