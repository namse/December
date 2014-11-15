//
//  MobClick.h
//  MobClick
//
//  Created by Aladdin on 2010-03-25.
//  Updated by Minghua on 2013-08-08.
//  Copyright 2010-2012 Umeng.com . All rights reserved.
//  Version 2.2.1.OpenUDID, updated_at 2013-09-12.

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#define UMOnlineConfigDidFinishedNotification @"OnlineConfigDidFinishedNotification"
#define XcodeAppVersion [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"]

typedef enum {
    REALTIME = 0,       //若?뜹?
    BATCH = 1,          //??ⓨ?
    SENDDAILY = 4,      //驪?ε?
    SENDWIFIONLY = 5,   //餓??IFI訝??ⓩ뜹?
    SEND_INTERVAL = 6,   //??弱?닻??
    SEND_ON_EXIT = 7        //??뷸瓦?ε?경뜹?
} ReportPolicy;

@protocol MobClickDelegate;
@class CLLocation;

/** MobClick?瀯溫←?멨?映삼??ц벴訝?誤若堊?竊???방?餓η굳?방??壤℡??堊.
    ????嶺?ζREALTIME,BATCH,SENDDAILY,SENDWIFIONLY,SEND_INTERVAL,SEND_ON_EXIT?
    ?뜸릎REALTIME,SENDWIFIONLY ?ゅⓩÆ??ⓨDEBUG與▼?訝??竊???elease與▼?鴉?ゅⓩ방BATCH?
    ?념???嶺?η瘟?댐?瑥룟鰲?념???嶺?ε??嶺?ε?당瑥닸
    http://blog.umeng.com/index.php/2012/12/0601/
    SEND_INTERVAL 訝뷸?弱?닻??,容溫ㅴ맏10燁,??쇠?답맏10 ??86400(訝鸚?竊 倻?訝?②?訝ゅ븅당瑥竊鴉?10溫양쉰?
    SEND_ON_EXIT 訝븅?뷸瓦?ε?경뜹?,瓦燁??嶺?ε?pp瓦烏瓦葉訝訝??竊野밧?????ⓩ루壤긷?弱?
    訝瓦瓦燁??嶺?εゅ?OS > 4.0?뜻鴉??, iOS < 4.0 鴉熬ヨゅ②??답맏BATCH?

 */
@interface MobClick : NSObject <UIAlertViewDelegate> {
@private
    id _internal;
}
#pragma mark basics

///---------------------------------------------------------------------------------------
/// @name  溫양쉰
///---------------------------------------------------------------------------------------

/** 溫양쉰app??у룔?긴???꿨??誤?xcode3藥η??쇔???????Build??CFBundleVersion),倻??誤?App Store訝???т????誤瘟?ⓩㅶ방??
 
 @param appVersion ??у뤄?堊倻溫양쉰?`XcodeAppVersion`.
 @return void.
*/
+ (void)setAppVersion:(NSString *)appVersion;


/** 凉?CrashReport?띌, 容溫ㅶ凉??뜻.
 
 @param value 溫양쉰?NO,弱긷餓ε녜??CrashReport?띌.
 @return void.
*/
+ (void)setCrashReportEnabled:(BOOL)value;


/** 溫양쉰?????dk?log岳→,容溫ㅴ?凉?
 @param value 溫양쉰訝?ES,umeng SDK 鴉渦??og岳→,溫겼?release雅㎩?띈?溫양쉰?NO.
 @return .
 @exception .
 */

+ (void)setLogEnabled:(BOOL)value;


///---------------------------------------------------------------------------------------
/// @name  凉?瀯溫?
///---------------------------------------------------------------------------------------


/** 凉???瀯溫?容溫ㅴ빳BATCH?밧???log.
 
 @param appKey ??appKey.
 @param reportPolicy ??嶺??
 @param channelId 歷??燁?訝?il?@""??容溫ㅴ?熬ヨ˙壤鵝@"App Store"歷?
 @return void
*/
+ (void)startWithAppkey:(NSString *)appKey;
+ (void)startWithAppkey:(NSString *)appKey reportPolicy:(ReportPolicy)rp channelId:(NSString *)cid;

/** 壤reportPolicy == SEND_INTERVAL ?띈얍log???닻
 
 @param second ?鵝訝븀?,?弱訝?0,?鸚㏛맏86400(訝鸚?.
 @return void.
*/

+ (void)setLogSendInterval:(double)second;


///---------------------------------------------------------------------------------------
/// @name  窈들?→?
///---------------------------------------------------------------------------------------


/** 窈들€띌욜?溫?溫겼??訝?iew熬ユ凉鸚?욘띌??餓θゅ런溫→뜸??餓θ???eginLogPageView,endLogPageView?ゅ②→?
 
 @param pageName ?誤溫겼??띌욜view?燁?
 @param seconds 燁?곤?int?.
 @return void.
*/

+ (void)logPageView:(NSString *)pageName seconds:(int)seconds;
+ (void)beginLogPageView:(NSString *)pageName;
+ (void)endLogPageView:(NSString *)pageName;

#pragma mark event logs


///---------------------------------------------------------------------------------------
/// @name  雅餓띄?溫?
///---------------------------------------------------------------------------------------


/** ?ゅ阿雅餓??곈瀯溫?
    鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D
 
 @param  eventId 營塋訝力ⓨ?雅餓?d.
 @param  label ?映삥嶺얇訝???嶺얌???ヨ?烏瀯溫∽??밥씩?訝雅餓띄訝??嶺양野방,訝?il?令뷴寧╊림?뜹?겻????eventId????嶺?
 @param  accumulation 榮??쇈訝뷴弱營瀯雅ㅴ?竊?餓θよ?野방訝雅餓?D??訝?映삥嶺얕?烏榮?竊?鴉?ζА?겻?訝뷴?겹
 @return void.
 */
+ (void)event:(NSString *)eventId; //嶺?雅 event:eventId label:eventId;
/** ?ゅ阿雅餓??곈瀯溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D
 */
+ (void)event:(NSString *)eventId label:(NSString *)label; // label訝?il?@""?띰?嶺?雅 event:eventId label:eventId;
/** ?ゅ阿雅餓??곈瀯溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D
 */
+ (void)event:(NSString *)eventId acc:(NSInteger)accumulation;
/** ?ゅ阿雅餓??곈瀯溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D
 */
+ (void)event:(NSString *)eventId label:(NSString *)label acc:(NSInteger)accumulation;
/** ?ゅ阿雅餓??곈瀯溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D
 */
+ (void)event:(NSString *)eventId attributes:(NSDictionary *)attributes;

/** ?ゅ阿雅餓??띌욜?溫?
    鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
    beginEvent,endEvent誤?野밥슴??阿?餓θゅ런溫→뜹?瓦durations??겻??瓦??
 
 @param  eventId 營塋訝力ⓨ?雅餓?d.
 @param  label ?映삥嶺얇訝???嶺얌???ヨ?烏瀯溫∽??밥씩?訝雅餓띄訝??嶺양野방,訝?il?令뷴寧╊림?뜹?겻????eventId????嶺?
 @param  primarykey 瓦訝ゅ?곁ⓧ??event_id訝壅룡鹽뷰?訝ゅ訝雅餓띰?亮뜸?鴉熬ョ?溫∽?野밥??訝訝や?餓뜹?eginEvent?endEvent 訝誤鴉??멨?eventId ? primarykey
 @param millisecond ?ゅ런溫→띌誤?瑥?誤鴉驪ョ?瓦??
 @return void.
 
 
 @warning 驪訝?vent?attributes訝?썼?瓦10訝?
    eventId?attributes訝key?value?썰??썰슴?①㈉?쇔?방耶寧??eventId?attributes?key?鸚㏛맏128訝?ytes(128訝よ길??겼?42訝ゅ랩?녔?耶)?label?attributes?value?鸚㏛맏256訝?ytes(256訝よ길??겼?84訝ゅ랩?녔?耶),
       擁瓦?弱熬ユょ??뜸릎eventId擁瓦?弱?凉訝????
    id竊 ts竊 du?岳?耶餘듸?訝?썰?訝?ventId?key??燁?

*/
+ (void)beginEvent:(NSString *)eventId;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */
+ (void)endEvent:(NSString *)eventId;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)beginEvent:(NSString *)eventId label:(NSString *)label;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)endEvent:(NSString *)eventId label:(NSString *)label;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)beginEvent:(NSString *)eventId primarykey :(NSString *)keyName attributes:(NSDictionary *)attributes;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)endEvent:(NSString *)eventId primarykey:(NSString *)keyName;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)event:(NSString *)eventId durations:(int)millisecond;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)event:(NSString *)eventId label:(NSString *)label durations:(int)millisecond;
/** ?ゅ阿雅餓??띌욜?溫?
 鵝욜ⓨ竊瑥룟?겼?App嶸←??곁溫양쉰->煐渦?ゅ阿雅餓?訝曆삣?멨??雅餓?D竊?뜹?ⓨ램葉訝鴉?η멨??雅餓?D.
 */

+ (void)event:(NSString *)eventId attributes:(NSDictionary *)attributes durations:(int)millisecond;


///---------------------------------------------------------------------------------------
/// @name  ?歷??ゅⓩ닸?
///---------------------------------------------------------------------------------------


/** ?歷??ゅⓩ닸경?役
    汝?ε??app????닸곤???닸겼섰??IAlertView?鹽븀ⓩ?壤?ⓩ루밧삣瀛㎪???pp鴉瓮녘쉬?경③??溫양쉰?營??
    ??닸겻??餓삡??鵝?
    ?③誤??ⓩ?▼①ク溫양쉰app??т에?竊容溫ㅶ???App Store.
    倻??ⓩ녘ゅ런?㎩띈ゅⓩ닸경鵝役葉竊瑥룟??obClickDelegate?appUpdate?방??
 
 @param title 野밧?UIAlertView?title.
 @param cancelTitle 野밧?UIAlertView?cancelTitle.
 @param otherTitle 野밧?UIAlertView?otherTitle.
 @param delegate ?誤?ゅ阿checkUpdate?野배괌.
 @param callBackSelectorWithDictionary 壤checkUpdate雅餓뜹??뜻ㅶ방?鴉熬ヨ?????뜻溫?pp?닸겻에??耶?멱˙鴉?.
 @return void.
 */

+ (void)checkUpdate;
/** ?歷??ゅⓩ닸경?役
 */

+ (void)checkUpdate:(NSString *)title cancelButtonTitle:(NSString *)cancelTitle otherButtonTitles:(NSString *)otherTitle;
/** ?歷??ゅⓩ닸경?役
 */

+ (void)checkUpdateWithDelegate:(id)delegate selector:(SEL)callBackSelectorWithDictionary;


///---------------------------------------------------------------------------------------
/// @name  ?①봇???
///---------------------------------------------------------------------------------------


/** 鵝욜ⓨ①봇??겼?쏙??餓θ⒳??ⓩ岳?밧??ⓧ릎???겼?
    汝?ε뭉?닸경?▼①ク?營??①봇???煐耶??NSUserDefaults standardUserDefaults]?,
    瘟?ⓩㅶ방??ⓨ??ゅⓩζ?①봇?닸?DK塋??嶺?η????③誤??ⓩ?▼①ク溫양쉰也썲①봇???
    瑥룟?MobClick startWithAppkey:]?방?阿?瘟??
    倻??녕ι?①봇??경???若?竊瑥루??MOnlineConfigDidFinishedNotification
 @param ?.
 @return void.
 */

+ (void)updateOnlineConfig;

/** 餓[NSUserDefaults standardUserDefaults]?룟煐耶??①봇??곁?겼?
    躍??곁?방??룟?訝?ey??쇽?訝躍??곁?룟????①봇???
    ?誤?瘟??pdateOnlineConfig??썰슴??倻??녕ι?①봇??경???若?竊瑥루??MOnlineConfigDidFinishedNotification
 
 @param key
 @return (NSString *) .
 */

+ (NSString *)getConfigParams:(NSString *)key;

/** 餓[NSUserDefaults standardUserDefaults]?룟煐耶??①봇???
  @return (NSDictionary *).
 */

+ (NSDictionary *)getConfigParams;


///---------------------------------------------------------------------------------------
/// @name ?곁鵝營溫양쉰
///---------------------------------------------------------------------------------------


/** 訝뷰??당꼐簾?瀯溫←ⓩ룟곁鵝營竊?餓θ??ⓩㅶ방?鴉?η?瀛у벧岳→
    ?誤?얏?CoreLocation.framework 亮뜸? #import <CoreLocation/CoreLocation.h>
 @param latitude 瀛у벧.
 @param longitude 瀯佯?
 @param location CLLocation *???곁岳→
 @return void
 */

+ (void)setLatitude:(double)latitude longitude:(double)longitude;
/** 訝뷰??당꼐簾?瀯溫←ⓩ룟곁鵝營竊?餓θ??ⓩㅶ방?鴉?η?瀛у벧岳→
 */

+ (void)setLocation:(CLLocation *)location;


///---------------------------------------------------------------------------------------
/// @name helper?방?
///---------------------------------------------------------------------------------------


/** ?ㅶ溫얍??????깍??ㅶ?방??방 apt?Cydia.app?path?εㅶ
 */
+ (BOOL)isJailbroken;
/** ?ㅶ鵝?App???˙?닺㎗
 */
+ (BOOL)isPirated;

#pragma mark DEPRECATED methods from version 1.7


/** 溫양쉰MobClick餓ｇ,藥꿰?startWithAppkey:??餓?訝誤?鵝욜⑨???餓ｇ?餓ε??
*/

+ (void)setDelegate:(id)delegate;
+ (void)setDelegate:(id)delegate reportPolicy:(ReportPolicy)rp;

/** 溫겼???ⓩ띌댐?與▼凉冶???訝誤?鵝욜⑨???餓ｇ?餓ε??
*/

+ (void)appLaunched;

/** 溫겼?饔餓띄?閭€띌댐?與▼瀯閭?訝誤?鵝욜⑨???餓ｇ?餓ε??
 */

+ (void)appTerminated;

/** ??與▼???
 [MobClick startWithAppkey:]?躍멨?pplication:didFinishLaunchingWithOptions:?熬ヨ??①??
 App??ⓨ??뷰?餓띰?倻?鵝亦→???pplication:didFinishLaunchingWithOptions:?曆삣???[MobClick startWithAppkey:]
 ?방?竊App???ⓧ?餓뜹?썰??力??э?閭ㅶ뜸?弱긷餓ζ?②???MobClick startSession:nil]?ε?ⓨ??session?
 ?躍멨??ⓩ雅寧т??방??띄??app?竊??app鵝욜ⓧ???
 
 */

+ (void)startSession:(NSNotification *)notification;


/** ?룟??sdk ??у?????與▼??ⓩ띈ゅ②???訝誤?鵝욜⑨???餓ｇ?餓ε??
 */

+ (NSString *)getAgentVersion;  //
@end

/** MobClickDelegate protocol
    閭ㅵ溫?訝訝ゆ방?訝?兩븃鵝욜⑨?兩븃?ⓩ경방?餓ｆ?
    + (void)startWithAppkey:(NSString *)appKey reportPolicy:(ReportPolicy)rp channelId:(NSString *)cid;
    + (void)checkUpdate:(id)delegate selector:(SEL)callBackSelector;

 */


@protocol MobClickDelegate <NSObject>
@optional
- (NSString *)appKey;
- (NSString *)channelId;
- (void)appUpdate:(NSDictionary *)appUpdateInfo;

@end