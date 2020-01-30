
import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtAccordion, AtList, AtListItem } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtAccordionTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { open: false, open2: false }
    }

    handleClick(type, value) {
        console.log("value:", value, "type:", type)

        if (type === 1) {
            this.setState({ open: value })
        } else {
            this.setState({ open2: value })
        }

    }

    render() {
        return (
            <View>
                <View>Accordion一般用法</View>
                <AtAccordion title='accordion1' open={this.state.open}
                    onClick={this.handleClick.bind(this, 1)}
                >
                    <AtList>
                        <AtListItem title='item1' extraText='extraText' arrow='right'
                            thumb='https://img12.360buyimg.com/jdphoto/s72x72_jfs/t6160/14/2008729947/2754/7d512a86/595c3aeeNa89ddf71.png'
                        ></AtListItem>
                        <AtListItem
                            title='标题文字'
                            note='描述信息'
                            arrow='right'
                            thumb='http://img10.360buyimg.com/jdphoto/s72x72_jfs/t5872/209/5240187906/2872/8fa98cd/595c3b2aN4155b931.png'
                        />
                        <AtListItem
                            title='标题文字'
                            note='描述信息'
                            extraText='详细信息'
                            arrow='right'
                            thumb='http://img12.360buyimg.com/jdphoto/s72x72_jfs/t10660/330/203667368/1672/801735d7/59c85643N31e68303.png'
                        />
                    </AtList>
                </AtAccordion>
                <View>Accordion图标</View>
                <AtAccordion
                    title='标题三'
                    open={this.state.open2}
                    icon={{ value: 'chevron-down', color: 'red', size: '15' }}
                    onClick={this.handleClick.bind(this, 2)}
                >
                    <AtList hasBorder={false}>
                        <AtListItem
                            title='标题文字'
                            arrow='right'
                            thumb='https://img12.360buyimg.com/jdphoto/s72x72_jfs/t6160/14/2008729947/2754/7d512a86/595c3aeeNa89ddf71.png'
                        />
                        <AtListItem
                            title='标题文字'
                            note='描述信息'
                            arrow='right'
                            thumb='http://img10.360buyimg.com/jdphoto/s72x72_jfs/t5872/209/5240187906/2872/8fa98cd/595c3b2aN4155b931.png'
                        />
                        <AtListItem
                            title='标题文字'
                            note='描述信息'
                            extraText='详细信息'
                            arrow='right'
                            thumb='http://img12.360buyimg.com/jdphoto/s72x72_jfs/t10660/330/203667368/1672/801735d7/59c85643N31e68303.png'
                        />
                    </AtList>
                </AtAccordion>
            </View>
        )
    }
}