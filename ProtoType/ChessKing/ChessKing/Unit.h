//
//  Unit.h
//  ChessKing
//
//  Created by echo on 2014. 10. 14..
//  Copyright 2014년 echo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cocos2d.h"

#define UNIT_SIZE 60.f
enum PlayerType
{
    PLAYER_NPC = 0,
    PLAYER_1 = 1,
    PLAYER_2 = 2,
};
enum UnitType
{
    NPC,
    PAWN,
    HORSE,
    BISHOP,
    KING,
};
@interface Unit : CCNode
{
    CCLabelTTF* label_Body;
    CCLabelTTF* label_HP;
    CCLabelTTF* label_Damage;
    CCLabelTTF* label_weight;
    CCLabelTTF* label_velocity;
    
    CGPoint hexaPosition;
    enum PlayerType  owner;
    int velocity;
    int damage;
    int hp;
    int weight;
    enum UnitType unitType;
    bool isAlive;
}
+(id)UnitWithName:(NSString*)_name unitType:(enum UnitType)_unitType HP:(int)_hp Damage:(int)_damage Velocity:(int)_velocity weight:(int)_weight owner:(enum PlayerType)_owner HexaPosition:(CGPoint)_hexaPosition;
-(id)initWithName:(NSString*)_name unitType:(enum UnitType)_unitType HP:(int)_hp Damage:(int)_damage Velocity:(int)_velocity weight:(int)_weight owner:(enum PlayerType)_owner HexaPosition:(CGPoint)_hexaPosition;
@property (nonatomic,readwrite, assign) CGPoint hexaPosition;
@property (nonatomic,readwrite) int velocity;
@property (nonatomic, readwrite) enum PlayerType owner;
@property (nonatomic, readwrite) enum UnitType unitType;

@property (nonatomic, readwrite) int hp;
@property (nonatomic, readwrite) int damage;
@property (nonatomic, readwrite) int weight;
@property (nonatomic, readwrite) bool isAlive;

@end
