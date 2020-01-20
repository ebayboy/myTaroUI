
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtActionSheet, AtActionSheetItem } from 'taro-ui'

import 'taro-ui/dist/style/components/action-sheet.scss'

export default class AtActionSheetTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            isOpened: false
        }
    }

    handleCancel = () => {
        this.showToast('点击了取消按钮')
    }

    handleClick = () => {
        this.setState({ isOpened: true })
    }

    handleClose = () => {
        this.setState({ isOpened: false })
    }

    showToast(name) {
        Taro.showToast({
            icon: 'none',
            title: name
        })
    }

    render() {
        const isOpened = this.state

        return (
            <View>
                <View>
                    <AtButton onClick={this.handleClick}>
                        打开 ActionSheet
                    </AtButton>
                </View>
                <AtActionSheet isOpened='false'
                    cancelText='取消'
                    title='头部标题可以用通过转义字符换行'
                    onCancel={this.handleCancel}
                    onClose={this.handleClose}
                >
                    <AtActionSheetItem
                        onClick={this.showToast.bind(this, '点击了按钮一')}
                    >
                        按钮一
                    </AtActionSheetItem>
                    <AtActionSheetItem
                        onClick={this.showToast.bind(this, '点击了按钮二')}
                    >
                        按钮二
                    </AtActionSheetItem>
                </AtActionSheet>
            </View>
        )
    }
}