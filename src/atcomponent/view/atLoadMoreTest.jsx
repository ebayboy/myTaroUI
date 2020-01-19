/* 用在列表里面，做滚动加载使用 */
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtLoadMore } from 'taro-ui'

import './atLoadMoreTest.scss'

export default class AtLoadMoreTest extends Component {
    constructor() {
        super(...arguments)
        this.state = {
            status: 'more'
        }
    }

    handleClick() {
        //start loading
        this.setState({
            status: 'loading'
        })

        //异步请求数据
        setTimeout(() => {
            this.setState({
                status: 'noMore'
            })
        }, 2000);
    }

    render() {
        return (
            <View>
                <AtLoadMore onClick={this.handleClick.bind(this)}
                    status={this.state.status}>
                </AtLoadMore>
            </View>
        )
    }
}