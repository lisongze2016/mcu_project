/*
 * This file holds USB constants and structures that are needed for
 * USB device APIs.  These are used by the USB device model, which is
 * defined in chapter 9 of the USB 2.0 specification and in the
 * Wireless USB 1.0 (spread around).  Linux has several APIs in C that
 * need these:
 *
 * - the master/host side Linux-USB kernel driver API;
 * - the "usbfs" user space API; and
 * - the Linux "gadget" slave/device/peripheral side driver API.
 *
 * USB 2.0 adds an additional "On The Go" (OTG) mode, which lets systems
 * act either as a USB master/host or as a USB slave/device.  That means
 * the master and slave side APIs benefit from working well together.
 *
 * There's also "Wireless USB", using low power short range radios for
 * peripheral interconnection but otherwise building on the USB framework.
 *
 * Note all descriptors are declared '__attribute__((packed))' so that:
 *
 * [a] they never get padded, either internally (USB spec writers
 *     probably handled that) or externally;
 *
 * [b] so that accessing bigger-than-a-bytes fields will never
 *     generate bus errors on any platform, even when the location of
 *     its descriptor inside a bundle isn't "naturally aligned", and
 *
 * [c] for consistency, removing all doubt even when it appears to
 *     someone that the two other points are non-issues for that
 *     particular descriptor type.
 */
 
#ifndef __CH9_H__
#define __CH9_H__



/**
 * USB标准请求数据结构
 * struct usb_ctrlrequest - SETUP data for a USB device control request
 * @bmRequestType: matches the USB bmRequestType field
 *								D7: Data transfer direction
 *										0 = Host-to-device
 *										1 = Device-to-host
 *								D6~D5: Type
 *												0 = Standard
 *												1 = Class
 *												2 = Vendor
 *												3 = Reserved
 *								D4~D5: Recipient
 *												0 = Device
 *												1 = Interface
 *												2 = Endpoint
 *												3 = Other
 *												4~31 = Reserved
 * @bRequest: matches the USB bRequest field
 * @wValue: matches the USB wValue field (le16 byte order)
 * @wIndex: matches the USB wIndex field (le16 byte order)
 * @wLength: matches the USB wLength field (le16 byte order)
 *
 * This structure is used to send control requests to a USB device.  It matches
 * the different fields of the USB 2.0 Spec section 9.3, table 9-2.  See the
 * USB spec for a fuller description of the different fields, and what they are
 * used for.
 *
 * Note that the driver for any interface can issue control requests.
 * For most devices, interfaces don't coordinate with each other, so
 * such requests may be made at any time.
 */
struct usb_ctrlrequest {
	__u8 bmRequestType;			//请求的特性 
	__u8 bRequest;					//请求代码
	__u16 wValue;						//小端结构(__le16)
	__u16 wIndex;						//小端结构(__le16)
	__u16 wLength;					//小端结构(__le16)
};

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS				0x00
#define USB_REQ_CLEAR_FEATURE			0x01
#define USB_REQ_SET_FEATURE				0x03
#define USB_REQ_SET_ADDRESS				0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE			0x0A
#define USB_REQ_SET_INTERFACE			0x0B
#define USB_REQ_SYNCH_FRAME				0x0C

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05

/*单片机是8位，不需要考虑字节对齐*/

/*-------------------------------------------------------------------------*/
/* USB_DT_DEVICE: Device descriptor */
struct usb_device_descriptor {
	__u8  bLength;							//该描述符的长度，设备描述符长度为18字节
	__u8  bDescriptorType;			//该描述符的类型，设备描述符的类型编号为0x01

	__u16 bcdUSB;								//该设备所使用的USB协议版本，小端结构(__le16)
	__u8  bDeviceClass;					//设备所使用的类代码，对于大多数标准的设备类，该字段设为0，bDeviceClass为0时，bDeviceSubClass也必须为0，bDeviceClass为0xff时，表示厂商自定义设备类
	__u8  bDeviceSubClass;			//设备使用的子类代码
	__u8  bDeviceProtocol;			//设备使用的协议代码，协议代码由USB协会规定，0：设备不使用类所定义的协议，FF：厂商自定义协议
	__u8  bMaxPacketSize0;			//端点0的最大包长，可取值8，16，32，64
	__u16 idVendor;							//厂商ID号，该ID号由USB协会分配，小端结构(__le16)
	__u16 idProduct;						//产品ID号，小端结构(__le16)
	__u16 bcdDevice;						//设备版本号，小端结构(__le16)
	__u8  iManufacturer;				//描述厂商字符串的索引值
	__u8  iProduct;							//描述产品字符串的索引值
	__u8  iSerialNumber;				//设备序列号字符串的索引值
	__u8  bNumConfigurations;		//表示设备有多少种配置，每种配置都会有一个配置描述符，主机通过设置配置来选择某一种配置，大部分USB设置只有一种配置，即该字段为1
};

#define USB_DT_DEVICE_SIZE		18

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information. */
struct usb_config_descriptor {
	__u8  bLength;				//该描述符的长度(9字节)
	__u8  bDescriptorType;		//描述符类型，配置描述符为0x02

	__u16 wTotalLength;			//配置描述符集合总长度，小端结构(__le16)
	__u8  bNumInterfaces;		//该配置所支持的接口数
	__u8  bConfigurationValue;	//该配置的值
	__u8  iConfiguration;		//描述该配置的字符串的索引值
	__u8  bmAttributes;			//该设备的属性
	__u8  bMaxPower;			//设备所需要的电流(单位为2mA)
};// __attribute__ ((packed));

#define USB_DT_CONFIG_SIZE		9
/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE		(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define USB_CONFIG_ATT_BATTERY		(1 << 4)	/* battery powered */

/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE: Interface descriptor */
struct usb_interface_descriptor {
	__u8  bLength;				//该描述符的长度(9字节)
	__u8  bDescriptorType;		//描述符类型，接口描述符为0x04

	__u8  bInterfaceNumber;		//该接口的编号(从0开始)
	__u8  bAlternateSetting;	//该接口的备用编号
	__u8  bNumEndpoints;		//该接口所使用的端点数
	__u8  bInterfaceClass;		//该接口所使用的类
	__u8  bInterfaceSubClass;	//该接口所使用的子类
	__u8  bInterfaceProtocol;	//该接口所使用的协议
	__u8  iInterface;			//描述该接口的字符串的索引值
};// __attribute__ ((packed));

#define USB_DT_INTERFACE_SIZE		9

/*-------------------------------------------------------------------------*/

/* USB_DT_ENDPOINT: Endpoint descriptor */
struct usb_endpoint_descriptor {
	__u8  bLength;				//该描述符的长度(7字节)
	__u8  bDescriptorType;		//描述符类型，接口描述符为0x05

	__u8  bEndpointAddress;		//该端点的地址	D7:该端点的传输方向(1:输入 0:输出) D6~D4:保留位 D3~D0:端点号
	__u8  bmAttributes;			//该端点的属性 D1~D0:传输类型(0:控制传输 1:等时传输 2:批量传输 3:中断传输)，
								//如果该端点为非等时传输,D7~D2为保留位，设为0
								//如果该端点为等时传输，D3~D2:同步类型(0:无同步 1:异步 2:适配 3:同步) D5~D4:用途(0:数据端点 1:反馈端点 2:暗含反馈的数据端点 3:保留位) D7~D6为保留位
	__u16 wMaxPacketSize;		//该端点支持的最大包长度,小端结构(__le16)
								//对于全速和低速模式，D10~D0表示端点的最大包长
								//对于高速模式，D12~D10表示每个帧附加的传输次数
	__u8  bInterval;			//端点的查询时间，对于中断端点，表示查询的帧间隔数

	/* NOTE:  these two are _only_ in audio endpoints. */
	/* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
	//__u8  bRefresh;
	//__u8  bSynchAddress;
};// __attribute__ ((packed));

#define USB_DT_ENDPOINT_SIZE		7
//#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */

/*-------------------------------------------------------------------------*/

//include/linux/hid.h
struct hid_class_descriptor {
	__u8  bDescriptorType;		//下级描述符的类型
	__u16 wDescriptorLength;	//下级描述符的长度
};// __attribute__ ((packed));

struct hid_descriptor {
	__u8  bLength;				//该描述符的长度
	__u8  bDescriptorType;		//描述符类型，HID描述符为0x21
	__u16 bcdHID;				//HID协议的版本
	__u8  bCountryCode;			//设备所使用的国家代码
	__u8  bNumDescriptors;		//下级描述符的数量

	struct hid_class_descriptor desc[1];
};// __attribute__ ((packed));

#define USB_DT_HID_SIZE	9

struct UsbMouse_ConfigDescriptor {
	struct usb_config_descriptor *usb_config_desc;		//配置描述符
	struct usb_interface_descriptor *usb_interface_desc;	//接口描述符
	struct hid_descriptor *hid_desc;						//HID描述符
	struct usb_endpoint_descriptor *usb_endpoint_desc;	//端点描述符
};

/*-------------------------------------------------------------------------*/

/* USB_DT_STRING: String descriptor */
struct usb_string_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__u16 wData[1];		/* UTF-16LE encoded */
};//__attribute__ ((packed));

/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters.
 */

#endif