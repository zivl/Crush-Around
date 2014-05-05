

#import "SpriteLayer.h"

@implementation SpriteLayer

#pragma mark -
#pragma mark Selectors
- (NSUInteger)currentSpriteIndex {
    return ((SpriteLayer*)[self presentationLayer]).spriteIndex;
}

#pragma mark -
#pragma mark Constructors
+ (id)layerWithImage:(CGImageRef)image spriteSize:(CGSize)size {
    return [[self alloc] initWithImage:image spriteSize:size];
}

- (id)initWithImage:(CGImageRef)image spriteSize:(CGSize)size {
    self = [self initWithImage:image];
    if ( self ) {
        CGSize spriteSizeNormalized = CGSizeMake(size.width/CGImageGetWidth(image), size.height/CGImageGetHeight(image));
        self.bounds = CGRectMake(0, 0, size.width, size.height);
        self.contentsRect = CGRectMake(0, 0, spriteSizeNormalized.width, spriteSizeNormalized.height);
    }
    return self;
}

- (id)initWithImage:(CGImageRef)image {
    self = [super init];
    if ( self ) {
        self.contents = (__bridge id)image;
        self.spriteIndex = 1;
    }
    return self;
}

#pragma mark -
#pragma mark Animation Methods
+ (BOOL)needsDisplayForKey:(NSString *)key {
    return [key isEqualToString:@"spriteIndex"];
}

+ (id<CAAction>)defaultActionForKey:(NSString *)event {
    if ( [event isEqualToString:@"contentsRect"] ) {
        return (id<CAAction>)[NSNull null];
    }
    return [super defaultActionForKey:event];
}

- (void)display {
    NSUInteger currentSpriteIndex = [self currentSpriteIndex];
    if ( !currentSpriteIndex ) {
        return;
    }
    
    CGSize spriteSize = self.contentsRect.size;
    
    self.contentsRect = CGRectMake(
                                   ((currentSpriteIndex-1) % (int)(1.0f/spriteSize.width)) * spriteSize.width,
                                   ((currentSpriteIndex-1) / (int)(1.0f/spriteSize.width)) * spriteSize.height,
                                   spriteSize.width,
                                   spriteSize.height);
}

#pragma mark -
#pragma mark Animation Delegate
- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag {
    [self removeFromSuperlayer];
}

@end

