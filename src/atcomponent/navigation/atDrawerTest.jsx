import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtDrawer, AtButton } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtDrawerTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { show: false }
    }

    handleClose() {
        this.setState({ show: false })
    }

    handleClick() {
        this.setState({ show: true })
    }

    handleItemClick(index) {
        const ENV = Taro.getEnv()
        let content
        if (typeof index !== 'number') {
            content = ''
        } else {
            content = `你点击了第 ${+index + 1} 个项目`
        }
        if (ENV !== 'WEB') content && Taro.showModal({ content, showCancel: false })
        else content && alert(content)
    }

    render() {
        return (
            <View>
                <AtButton onClick={this.handleClick.bind(this)}>显示Drawer</AtButton>
                <AtDrawer
                    show={this.state.show}
                    mask
                    onClose={this.handleClose.bind(this)}
                    items={['item1', 'item2','item3']}
                    onItemClick={this.handleItemClick.bind(this)}
                />
            </View>
        )
    }
}