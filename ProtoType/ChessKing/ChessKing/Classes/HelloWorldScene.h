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
#define BISHOP_PAST_RADIUS (MOVABLE_RAIDUS * 1.5f)

#define MOVABLE_AREA_COLOR_ENEMY ccc4(255, 30, 30, 196)
#define MOVABLE_AREA_COLOR_ALIY ccc4(30, 128, 30, 196)
#define MOVABLE_AREA_COLOR_OUTSIDE ccc4(30,128,128, 196)
#define MOVABLE_AREA_COLOR_NORMAL ccc4(128, 30, 30, 196)
#define PAST_COLOR_BISHOP_ALREADY ccc4(255, 255, 255, 196)
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
        float duration;
    }animationQueue[MAX_ANIMATION_COUNT];
    int animationCount;
    
    NSMutableArray * freedomMoveQueue;
}
// -----------------------------------------------------------------------

+ (HelloWorldScene *)scene;
- (id)init;

- (void)showMovableArea:(CGPoint)startHexaPoint velocity:(int)velocity;
- (void)showMovableAreaWithFreeMove:(Unit*)target MoveQueue:(NSMutableArray*)moveQueue;
- (void)showNowMoveNode;
- (void)clearMovablePosition;
- (void)clearNowMoveNode;

-(void)generateMapByHardCoding;
-(void)initMapBorder;
-(void)initVerts;
-(bool)isInside:(CGPoint)hexaPoint;
-(bool)isEnableMovePosition:(CGPoint)hexaPoint;
-(void)generateUnitByHardCoding;
-(Unit*)getUnitInHexaPoint:(CGPoint)hexaPoint;

-(void)onMoveWithVectorType:(int)vectorType Length:(int)length Target:(Unit*)target isFirstMove:(bool)isFirstMove;
-(void)finishMove;
-(void)applyDamageWithAttacker:(Unit*)attacker Target:(Unit*)target;
-(void)pushByPusher:(Unit*)pusher Target:(Unit*)target lastLength:(int)lastLength isFirstPush:(bool)isFirstPush;
-(void)thisGuyKiiled:(Unit*)unit;

-(void)startAnimation;
-(void)nextAnimation;
-(void)finishAnimation;

-(void)removeAnimationUnit;

-(void)horeseJump:(Unit*)horse ToHexa:(CGPoint)hexa;
-(void)freeMove:(Unit*)target MoveQueue:(NSMutableArray*)moveQueue;
// -----------------------------------------------------------------------
@end