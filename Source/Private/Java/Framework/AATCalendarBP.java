package com.Monocular.AndroidAttachThing;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;

import android.content.ContentValues;
import android.net.Uri;
import android.text.TextUtils;
import android.provider.CalendarContract;
import android.database.Cursor;
import android.content.ContentUris;
import java.util.ArrayList;

@Keep
public class AATAATCalendarBP 
{
	private static String CALENDER_EVENT_URL = "content://com.android.calendar/events";
	private static String CALENDER_REMINDER_URL = "content://com.android.calendar/reminders";


	// 添加日历事件
	@Keep
	public static boolean PushAppointmentsToCalender(Activity activity, AGCalendar calendar)
	{
		if (TextUtils.isEmpty(calendar.Title))
		{
			return false;
		}
		
		ContentValues eventValues = new ContentValues();

		eventValues.put("calendar_id", 1);
		eventValues.put("title", calendar.Title);
		eventValues.put("description", calendar.AddInfo);
		//eventValues.put("eventLocation", calendar.Place);

		long endDate = calendar.StartDatetTime + 1000 * 60 * 60;
		eventValues.put(CalendarContract.Events.DTSTART, calendar.StartDatetTime);
		eventValues.put(CalendarContract.Events.DTEND, endDate);

		eventValues.put("eventStatus", calendar.EventStatus); 
		//eventValues.put("eventTimezone", "UTC/GMT +2:00");
		eventValues.put(CalendarContract.Events.EVENT_TIMEZONE, "Asia/Shanghai");

		eventValues.put(CalendarContract.Events.HAS_ALARM, 1); // 0 for false, 1 for true

		Uri eventUri = activity.getApplicationContext().getContentResolver().insert(Uri.parse(CALENDER_EVENT_URL), eventValues);
		long eventID = Long.parseLong(eventUri.getLastPathSegment());
		if (eventUri == null)
		{
			return false;
		}

		//long eventID = Long.parseLong(eventUri.getLastPathSegment());

		ContentValues reminderValues = new ContentValues();

			reminderValues.put(CalendarContract.Reminders.EVENT_ID, ContentUris.parseId(eventUri));
			reminderValues.put(CalendarContract.Reminders.MINUTES, 5);

			reminderValues.put(CalendarContract.Reminders.METHOD, 1);

			Uri reminderUri = activity.getApplicationContext().getContentResolver().insert(Uri.parse(CALENDER_REMINDER_URL), reminderValues);
			if (reminderUri == null)
			{
				return false;
			}

		return true;
	}

	// 删除日历事件
	@Keep
	public static boolean DeleteCalendarEvent(Activity activity, String InTitle, long InStartDatetTime)
	{
		Cursor eventCursor = activity.getApplicationContext().getContentResolver().query(Uri.parse(CALENDER_EVENT_URL), null, null, null, null);
		try 
		{
			if (eventCursor == null)
			{
				return false;
			}
			if (eventCursor.getCount() > 0)
			{
				for (eventCursor.moveToFirst(); !eventCursor.isAfterLast(); eventCursor.moveToNext())
				{
					if (IsHasCalendarEvent(eventCursor, InTitle, InStartDatetTime))
					{
						int id = eventCursor.getInt(eventCursor.getColumnIndex(CalendarContract.Calendars._ID));//取得id
						Uri deleteUri = ContentUris.withAppendedId(Uri.parse(CALENDER_EVENT_URL), id);
						int rows = activity.getApplicationContext().getContentResolver().delete(deleteUri, null, null);
                        if (rows == -1) //事件删除失败
						{ 
                            return false;
						}
						return true;
					}
				}
			}
			eventCursor.close();

			return false;
		} catch (Exception e) {
			eventCursor.close();
			e.printStackTrace();
			return false;
		}
	}

	// 查看有多少日程
	@Keep
	public static AGCalendar[] GetAllCalendarEvent(Activity activity)
	{
		Cursor eventCursor = activity.getApplicationContext().getContentResolver().query(Uri.parse(CALENDER_EVENT_URL), null, null, null, null);
		try 
		{
			if (eventCursor == null)
			{
				return new AGCalendar[0];
			}

			ArrayList<AGCalendar> calendarsArray = new ArrayList<>();

			for (eventCursor.moveToFirst(); !eventCursor.isAfterLast(); eventCursor.moveToNext())
			{
				if (eventCursor.getInt(eventCursor.getColumnIndex("calendar_id")) != 1 || eventCursor.getInt(eventCursor.getColumnIndex(CalendarContract.Events.HAS_ALARM)) != 1)
				{
					continue;
				}

				AGCalendar agCalendar = new AGCalendar();

				agCalendar.Title = eventCursor.getString(eventCursor.getColumnIndex("title"));
				agCalendar.AddInfo = eventCursor.getString(eventCursor.getColumnIndex("description"));
				agCalendar.Place = eventCursor.getString(eventCursor.getColumnIndex("eventLocation"));

				agCalendar.EventStatus = eventCursor.getInt(eventCursor.getColumnIndex("eventStatus"));
				agCalendar.StartDatetTime = eventCursor.getInt(eventCursor.getColumnIndex(CalendarContract.Events.DTSTART));

				calendarsArray.add(agCalendar);
			}

			eventCursor.close();
			return calendarsArray.toArray(new AGCalendar[0]);

		}
		catch (Exception e) {
			eventCursor.close();
			e.printStackTrace();
			return new AGCalendar[0];
		}
	}

	// 是否有该日历
	private static boolean IsHasCalendarEvent(Cursor eventCursor, String InTitle, long InStartDatetTime)
	{
		try 
		{
			if (eventCursor == null)
			{
				return false;
			}

			String eventTitle = eventCursor.getString(eventCursor.getColumnIndex("title"));
			long StartDatetTime = eventCursor.getLong(eventCursor.getColumnIndex(CalendarContract.Events.DTSTART));
			if (TextUtils.isEmpty(InTitle) || !InTitle.equals(eventTitle))
			{
				return false;
			}

			return StartDatetTime == InStartDatetTime;
				
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
	}
}