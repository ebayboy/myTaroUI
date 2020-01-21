
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtActionSheet, AtActionSheetItem, AtButton } from 'taro-ui'

//import 'taro-ui/dist/style/components/action-sheet.scss'
//import 'taro-ui/dist/style/components/button.scss'

import "taro-ui/dist/style/index.scss"

export default class AtActionSheetTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            isOpened: false
        }
    }

    handleCancel = () => {
        /*  1. 如果设置了handleCancle函数， 则必须手动设置 setState { isOpened:fasle}, /否则不会关闭窗口
            2. 如果没有设置handleCancle函数， 则点取消按钮会自动关闭窗口
        */
        this.showToast('点击了取消按钮')
        this.setState({ isOpened: false })
    }

    handleClick = () => {
        this.setState({ isOpened: true })
    }

    handleClickAction(type) {
        console.log("clickAction :", type);
        console.log("clickAction after isOpened value:", this.state.isOpened);
    }


    handleClose = () => {
        console.log("handleClose isClose value:", this.state.isOpened);
    }

    showToast(name) {
        Taro.showToast({
            icon: 'none',
            title: name
        })
    }

    render() {
        /* 错误的赋值： const isOpened = this.state */

        const isOpened = this.state.isOpened

        return (
            <View>
                <AtButton type='primary' onClick={this.handleClick.bind(this)}>Open ActionSheet</AtButton>
                <AtActionSheet isOpened={isOpened}
                    cancelText='取消'
                    title='头部标题可以用通过转义字符换行'
                    onCancel={this.handleCancel.bind(this)}
                    onClose={this.handleClose.bind(this)}
                >
                    <AtActionSheetItem
                        onClick={this.handleClickAction.bind(this, '点击了按钮一')}
                    >
                        按钮一
                    </AtActionSheetItem>
                    <AtActionSheetItem
                        onClick={this.handleClickAction.bind(this, '点击了按钮二')}
                    >
                        按钮二
                    </AtActionSheetItem>
                </AtActionSheet>
            </View>
        )
    }
}