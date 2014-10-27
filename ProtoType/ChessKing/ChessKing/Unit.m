//
//  Unit.m
//  ChessKing
//
//  Created by echo on 2014. 10. 14..
//  Copyright 2014년 echo. All rights reserved.
//

#import "Unit.h"


@implementation Unit
@synthesize hexaPosition;
@synthesize velocity;
@synthesize owner;
@synthesize unitType;
@synthesize hp;
@synthesize weight;
@synthesize damage;
@synthesize isAlive;

+(id)UnitWithName:(NSString *)_name unitType:(enum UnitType)_unitType HP:(int)_hp Damage:(int)_damage Velocity:(int)_velocity weight:(int)_weight owner:(enum PlayerType)_owner HexaPosition:(CGPoint)_hexaPosition
{
    return [[Unit alloc]initWithName:_name unitType:_unitType HP:_hp Damage:_damage Velocity:_velocity weight:_weight owner:_owner HexaPosition:_hexaPosition];
}
-(id)initWithName:(NSString*)_name unitType:(enum UnitType)_unitType HP:(int)_hp Damage:(int)_damage Velocity:(int)_velocity weight:(int)_weight owner:(enum PlayerType)_owner HexaPosition:(CGPoint)_hexaPosition
{
    if(self = [super init])
    {
        hp = _hp;
        damage = _damage;
        velocity = _velocity;
        weight = _weight;
        owner = _owner;
        hexaPosition = _hexaPosition;
        unitType=_unitType;
        isAlive = true;
        
        label_Body = [CCLabelTTF labelWithString:_name fontName:@"Verdana-Bold"fontSize:UNIT_SIZE];
        switch (owner) {
            case PLAYER_1:
                [label_Body setColor:[CCColor colorWithCcColor3b:ccc3(255, 102, 102)]];
                break;
                
            case PLAYER_2:
                [label_Body setColor:[CCColor colorWithCcColor3b:ccc3(102, 102, 255)]];
                break;
                
            default:
                break;
        }
        label_Body.position = ccp(0.f, 0.f);
        [self addChild:label_Body];
        
        
        label_HP = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",hp] fontName:@"Verdana-Bold"fontSize:UNIT_SIZE/4.f];
        [label_HP setColor:[CCColor colorWithCcColor3b:ccc3(220, 4, 4)]];
        label_HP.position = ccp(UNIT_SIZE * 0.4f, UNIT_SIZE * -0.4f);
        [self addChild:label_HP];
        
        label_Damage = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",damage] fontName:@"Verdana-Bold"fontSize:UNIT_SIZE/4.f];
        label_Damage.position = ccp(UNIT_SIZE * -0.4f, UNIT_SIZE * -0.4f);
        [label_Damage setColor:[CCColor colorWithCcColor3b:ccc3(230, 231, 232)]];
        [self addChild:label_Damage];
        
        label_weight = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",weight] fontName:@"Verdana-Bold"fontSize:UNIT_SIZE/4.f];
        label_weight.position = ccp(UNIT_SIZE * -0.4f, UNIT_SIZE * 0.4f);
        [label_weight setColor:[CCColor colorWithCcColor3b:ccc3(0x8B, 0x8D, 0x7A)]];
        [self addChild:label_weight];
        
        label_velocity = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",velocity] fontName:@"Verdana-Bold"fontSize:UNIT_SIZE/4.f];
        label_velocity.position = ccp(UNIT_SIZE * 0.4f, UNIT_SIZE * 0.4f);
        [label_velocity setColor:[CCColor colorWithCcColor3b:ccc3(0, 255, 128)]];
        [self addChild:label_velocity];
        
        /*
         label_HP.position = ccp(UNIT_SIZE * 1.f / 4.f, UNIT_SIZE * -1.f / 4.f);
         [self addChild:label_HP];
         
         label_Damage = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",damage] fontName:@"Verdana"fontSize:UNIT_SIZE/4.f];
         label_Damage.position = ccp(UNIT_SIZE * -1.f / 4.f, UNIT_SIZE * -1.f / 4.f);
         [label_Damage setColor:[CCColor colorWithCcColor3b:ccc3(230, 231, 232)]];
         [self addChild:label_Damage];
         
         label_weight = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",weight] fontName:@"Verdana"fontSize:UNIT_SIZE/4.f];
         label_weight.position = ccp(UNIT_SIZE * -1.f / 4.f, UNIT_SIZE * 1.f / 4.f);
         [label_weight setColor:[CCColor colorWithCcColor3b:ccc3(0x8B, 0x8D, 0x7A)]];
         [self addChild:label_weight];
         
         label_velocity = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d",velocity] fontName:@"Verdana"fontSize:UNIT_SIZE/4.f];
         label_velocity.position = ccp(UNIT_SIZE * 1.f / 4.f, UNIT_SIZE * 1.f / 4.f);
         [label_velocity setColor:[CCColor colorWithCcColor3b:ccc3(0, 255, 128)]];

         */
    }
    return self;
}
-(void)setHp:(int)_hp
{
    hp = _hp;
    [label_HP setString:[NSString stringWithFormat:@"%d",hp]];
}
@end
