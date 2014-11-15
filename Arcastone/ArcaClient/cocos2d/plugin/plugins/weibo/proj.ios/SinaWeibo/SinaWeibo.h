//
//  SinaWeibo.h
//  sinaweibo_ios_sdk
//
//  Created by Wade Cheng on 4/19/12.
//  Copyright (c) 2012 SINA. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SinaWeiboAuthorizeView.h"
#import "SinaWeiboRequest.h"

@protocol SinaWeiboDelegate;

@interface SinaWeibo : NSObject <SinaWeiboAuthorizeViewDelegate, SinaWeiboRequestDelegate>
{
    NSString *userID;
    NSString *accessToken;
    NSDate *expirationDate;
    id<SinaWeiboDelegate> delegate;
    
    NSString *appKey;
    NSString *appSecret;
    NSString *appRedirectURI;
    NSString *ssoCallbackScheme;
    
    SinaWeiboRequest *request;
    NSMutableSet *requests;
    BOOL ssoLoggingIn;
}

@property (nonatomic, copy) NSString *userID;
@property (nonatomic, copy) NSString *accessToken;
@property (nonatomic, copy) NSDate *expirationDate;
@property (nonatomic, copy) NSString *refreshToken;
@property (nonatomic, copy) NSString *ssoCallbackScheme;
@property (nonatomic, assign) id<SinaWeiboDelegate> delegate;

- (id)initWithAppKey:(NSString *)appKey appSecret:(NSString *)appSecrect
      appRedirectURI:(NSString *)appRedirectURI
         andDelegate:(id<SinaWeiboDelegate>)delegate;

- (id)initWithAppKey:(NSString *)appKey appSecret:(NSString *)appSecrect
      appRedirectURI:(NSString *)appRedirectURI
   ssoCallbackScheme:(NSString *)ssoCallbackScheme
         andDelegate:(id<SinaWeiboDelegate>)delegate;

- (void)applicationDidBecomeActive;
- (BOOL)handleOpenURL:(NSURL *)url;

// Log in using OAuth Web authorization.
// If succeed, sinaweiboDidLogIn will be called.
- (void)logIn;

// Log out.
// If succeed, sinaweiboDidLogOut will be called.
- (void)logOut;

// Check if user has logged in, or the authorization is expired.
- (BOOL)isLoggedIn;
- (BOOL)isAuthorizeExpired;


// isLoggedIn && isAuthorizeExpired
- (BOOL)isAuthValid;

- (SinaWeiboRequest*)requestWithURL:(NSString *)url
                             params:(NSMutableDictionary *)params
                         httpMethod:(NSString *)httpMethod
                           delegate:(id<SinaWeiboRequestDelegate>)delegate;

@end


/**
 * @description 寧т??밧??③若?경ㅵ溫竊?삣??뜸??ζㅷ굳野배괌竊?ⓧ?若??삣?瀯???瘟
 */
@protocol SinaWeiboDelegate <NSObject>

@optional

- (void)sinaweiboDidLogIn:(SinaWeibo *)sinaweibo;
- (void)sinaweiboDidLogOut:(SinaWeibo *)sinaweibo;
- (void)sinaweiboLogInDidCancel:(SinaWeibo *)sinaweibo;
- (void)sinaweibo:(SinaWeibo *)sinaweibo logInDidFailWithError:(NSError *)error;
- (void)sinaweibo:(SinaWeibo *)sinaweibo accessTokenInvalidOrExpired:(NSError *)error;

@end

extern BOOL SinaWeiboIsDeviceIPad();
