import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtTabs, AtTabsPane } from 'taro-ui'
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
                    <AtTabsPane current={this.state.current} index={0} >
                        <View style='padding: 100px 50px;background-color: #FAFBFC;text-align: center;' >标签页一的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={1}>
                        <View style='padding: 100px 50px;background-color: #FAFBFC;text-align: center;'>标签页二的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={2}>
                        <View style='padding: 100px 50px;background-color: #FAFBFC;text-align: center;'>标签页三的内容</View>
                    </AtTabsPane>
                </AtTabs>
            </View>
        )
    }
}