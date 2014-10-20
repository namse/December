//
//  HelloWorldScene.h
//  ChessKing
//
//  Created by echo on 2014. 10. 14..
//  Copyright echo 2014년. All rights reserved.
//
// -----------------------------------------------------------------------

// Importing cocos2d.h and cocos2d-ui.h, will import anything you need to start using Cocos2D v3
#import "cocos2d.h"
#import "cocos2d-ui.h"
#import "Unit.h"
// -----------------------------------------------------------------------

/**
 *  The main scene
 */

#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define MAP_RADIUS 50.f
#define MOVABLE_RAIDUS 10.f
#define MOVE_DURATION 0.3f
#define DIE_DURATION 0.3f
#define JUMP_DURATION 0.3f
#define MAX_ANIMATION_COUNT 100
//#define HEXA_INDEX_SHOW

@interface HelloWorldScene : CCScene
{
    CGPoint verts[MAP_WIDTH][MAP_HEIGHT][6];
    NSMutableArray *unitList;
    
    bool isPicked;
    Unit* pickedUnit;
    
    CCDrawNode* movableFocusArea;
    NSMutableArray * moveableAreaList;
    CCDrawNode* nowMoveNode;
    CGPoint nowMovePoint;
    
    CCDrawNode* mapBorder;
    NSMutableArray * mapNodeList;
    
    enum PlayerType whoTurn;
    enum AnimationType{
        HORSEJUMP,
        MOVETO,
        DIE,
    };
    struct AnimationNode{
        __unsafe_unretained CCNode* node;
        enum AnimationType type;
        CGPoint moveTo;
        ccBezierConfig jumpConfig;
    }animationQueue[MAX_ANIMATION_COUNT];
    int animationCount;
    
}
// -----------------------------------------------------------------------

+ (HelloWorldScene *)scene;
- (id)init;

- (void)showMovableArea:(CGPoint)startHexaPoint velocity:(int)velocity;
- (void)showNowMoveNode;
- (void)clearMovablePosition;
- (void)clearNowMoveNode;

-(void)generateMapByHardCoding;
-(void)initMapBorder;
-(void)initVerts;
-(bool)isEnablePosition:(CGPoint)hexaPoint;
-(bool)isEnableMovePosition:(CGPoint)hexaPoint;
-(void)generateUnitByHardCoding;
-(Unit*)getUnitInHexaPoint:(CGPoint)hexaPoint;

-(int)getVectorTypeByFrom:(CGPoint)fromHexaPoint To:(CGPoint)toHexaPoint;
-(void)onMoveWithVectorType:(int)vectorType Length:(int)length Target:(Unit*)target isFirstMove:(bool)isFirstMove;
-(void)finishMove;
-(void)applyDamageWithAttacker:(Unit*)attacker Target:(Unit*)target;
-(void)pushByPusher:(Unit*)pusher Target:(Unit*)target lastLength:(int)lastLength isFirstPush:(bool)isFirstPush;
-(void)thisGuyKiiled:(Unit*)unit;
-(int)getVectorSizeFrom:(CGPoint)fromHexaPoint To:(CGPoint)toHexaPoint;

-(void)startAnimation;
-(void)nextAnimation;
-(void)finishAnimation;

-(void)removeAnimationUnit;

-(void)horeseJump:(Unit*)horse ToHexa:(CGPoint)hexa;
// -----------------------------------------------------------------------
@end