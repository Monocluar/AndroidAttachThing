package com.Monocular.AndroidAttachThing;

public class AATCalendar {
	String Title;
	String AddInfo;
	String Place; // 位置
	int EventStatus; //This information is sufficient for most entries tentative (0),confirmed (1) or canceled
	long StartDatetTime; // 起始时间
	boolean bNeedMailService; // 是否需要邮件服务
}