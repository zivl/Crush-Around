
#import <QuartzCore/QuartzCore.h>

// this class represents a sprite animation effect

@interface SpriteLayer : CALayer

@property (readwrite, nonatomic) NSUInteger spriteIndex;

+ (id)layerWithImage:(CGImageRef)image spriteSize:(CGSize)size;

- (NSUInteger)currentSpriteIndex;

@end
