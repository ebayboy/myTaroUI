import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtTabs } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtTabsTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 0 }
    }

    handleClick(value) {
        this.setState({ current: value })
    }

    render() {
        return (
            <View>
                <AtTabs
                    current={this.state.current}
                    tabList={[{ title: 'tab1' }, { title: 'tab2' }, { title: 'tab3' }]}
                    onClick={this.handleClick.bind(this)}
                >
                </AtTabs>
            </View>
        )
    }
}