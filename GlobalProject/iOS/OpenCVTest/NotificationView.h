//
//  NotificationView.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/5/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NotificationView : UIView

@property (nonatomic, weak) IBOutlet UIImageView *bg;
@property (nonatomic, weak) IBOutlet UILabel *message;
@property (nonatomic, weak) IBOutlet UIButton *okButton;

-(void)showNotificationWithMessage:(NSString *)iMessgae;
-(void)hideNotificationMessage;

@end
