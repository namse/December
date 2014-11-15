//
//  SinaWeiboRequest.h
//  sinaweibo_ios_sdk
//  Based on OAuth 2.0
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//
//  Copyright 2011 Sina. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SinaWeiboRequest;
@class SinaWeibo;


/**
 * @description 寧т??밧??②욥孃?API?뜹?경ㅶㅵ溫竊壤sdk若?api?溫욥??瓦鴉?η閭ㅷ굳野배괌若??εｈ욥瀯???瘟竊佯?ⓨⓨ溫若?곁?멨??방?訝?ζ띈욥瀯?亮뜹野밧?鸚??
 */
@protocol SinaWeiboRequestDelegate <NSObject>
@optional
- (void)request:(SinaWeiboRequest *)request didReceiveResponse:(NSURLResponse *)response;
- (void)request:(SinaWeiboRequest *)request didReceiveRawData:(NSData *)data;
- (void)request:(SinaWeiboRequest *)request didFailWithError:(NSError *)error;
- (void)request:(SinaWeiboRequest *)request didFinishLoadingWithResult:(id)result;
@end

@interface SinaWeiboRequest : NSObject
{
    SinaWeibo                       *sinaweibo;//weak reference
    
    NSString                        *url;
    NSString                        *httpMethod;
    NSDictionary                    *params;
    
    NSURLConnection                 *connection;
    NSMutableData                   *responseData;
    
    id<SinaWeiboRequestDelegate>    delegate;
}

@property (nonatomic, assign) SinaWeibo *sinaweibo;
@property (nonatomic, retain) NSString *url;
@property (nonatomic, retain) NSString *httpMethod;
@property (nonatomic, retain) NSDictionary *params;
@property (nonatomic, assign) id<SinaWeiboRequestDelegate> delegate;

+ (SinaWeiboRequest *)requestWithURL:(NSString *)url 
                          httpMethod:(NSString *)httpMethod 
                              params:(NSDictionary *)params
                            delegate:(id<SinaWeiboRequestDelegate>)delegate;

+ (SinaWeiboRequest *)requestWithAccessToken:(NSString *)accessToken
                                         url:(NSString *)url
                                  httpMethod:(NSString *)httpMethod 
                                      params:(NSDictionary *)params
                                    delegate:(id<SinaWeiboRequestDelegate>)delegate;

+ (NSString *)getParamValueFromUrl:(NSString*)url paramName:(NSString *)paramName;
+ (NSString *)serializeURL:(NSString *)baseURL params:(NSDictionary *)params httpMethod:(NSString *)httpMethod;

- (void)connect;
- (void)disconnect;

@end
