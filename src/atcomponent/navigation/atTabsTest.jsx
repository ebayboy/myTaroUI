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
                <View>一般tabs用法</View>
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
                <View>scroll标签用法</View>
                <AtTabs
                    current={this.state.current}
                    scroll
                    tabList={[
                        { title: '标签页1' },
                        { title: '标签页2' },
                        { title: '标签页3' },
                        { title: '标签页4' },
                        { title: '标签页5' },
                        { title: '标签页6' }
                    ]}
                    onClick={this.handleClick.bind(this)}>
                    <AtTabsPane current={this.state.current} index={0}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页一的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={1}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页二的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={2}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页三的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={3}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页四的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={4}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页五的内容</View>
                    </AtTabsPane>
                    <AtTabsPane current={this.state.current} index={5}>
                        <View style='font-size:18px;text-align:center;height:100px;'>标签页六的内容</View>
                    </AtTabsPane>
                </AtTabs>
                <View>垂直标签</View>
                <AtTabs
                    current={this.state.current}
                    scroll
                    height='200px'
                    tabDirection='vertical'
                    tabList={[
                        { title: '标签页1' },
                        { title: '标签页2' },
                        { title: '标签页3' },
                        { title: '标签页4' },
                        { title: '标签页5' },
                        { title: '标签页6' },
                    ]}
                    onClick={this.handleClick.bind(this)}>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={0}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页一的内容</View>
                    </AtTabsPane>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={1}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页二的内容</View>
                    </AtTabsPane>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={2}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页三的内容</View>
                    </AtTabsPane>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={3}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页四的内容</View>
                    </AtTabsPane>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={4}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页五的内容</View>
                    </AtTabsPane>
                    <AtTabsPane tabDirection='vertical' current={this.state.current} index={5}>
                        <View style='font-size:18px;text-align:center;height:200px;'>标签页六的内容</View>
                    </AtTabsPane>
                </AtTabs>
                <Vieww>禁止内容切换动画</Vieww>
                <AtTabs
                    animated={false}
                    current={this.state.current}
                    tabList={[
                        { title: '标签页1' },
                        { title: '标签页2' },
                        { title: '标签页3' }
                    ]}
                    onClick={this.handleClick.bind(this)}>
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